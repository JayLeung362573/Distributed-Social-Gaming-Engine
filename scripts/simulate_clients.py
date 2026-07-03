#!/usr/bin/env python3

import argparse
import asyncio
import time
from dataclasses import dataclass

import websockets
from websockets.exceptions import ConnectionClosed


@dataclass
class ClientResult:
    client_id: int
    connected: bool
    completed: bool
    messages_sent: int
    valid_messages_sent: int
    malformed_messages_sent: int
    responses_received: int
    expected_responses: int
    response_complete: bool
    error: str = ""


def build_message(client_id: int, message_index: int) -> tuple[str, bool]:
    """
    Build a repeatable workload containing four valid protocol messages
    followed by one intentionally malformed message.

    Returns:
        A tuple of (payload, is_malformed).
    """
    message_type = message_index % 5

    if message_type == 0:
        return "BrowseLobbies0", False

    if message_type == 1:
        return "GetLobbyState", False

    if message_type == 2:
        return f"StartGame:LoadClient{client_id}", False

    if message_type == 3:
        return f"LeaveLobbyLoadClient{client_id}", False

    return "DeleteServer:now", True


async def collect_responses(
    websocket,
    expected_responses: int,
    timeout_seconds: float,
) -> int:
    """
    Receive responses until the workload-specific expected count is reached
    or the response timeout expires.
    """
    responses_received = 0
    loop = asyncio.get_running_loop()
    deadline = loop.time() + timeout_seconds

    while responses_received < expected_responses:
        remaining_seconds = deadline - loop.time()

        if remaining_seconds <= 0:
            break

        try:
            await asyncio.wait_for(
                websocket.recv(),
                timeout=remaining_seconds,
            )
            responses_received += 1

        except asyncio.TimeoutError:
            break

        except ConnectionClosed:
            break

    return responses_received

async def simulate_client(
    client_id: int,
    uri: str,
    messages_per_client: int,
    delay_seconds: float,
    response_timeout_seconds: float,
    connect_timeout_seconds: float,
) -> ClientResult:
    connected = False
    completed = False
    response_complete = False
    messages_sent = 0
    valid_messages_sent = 0
    malformed_messages_sent = 0
    responses_received = 0
    error = ""
    response_task = None

    valid_messages_planned = sum(
        1
        for message_index in range(messages_per_client)
        if not build_message(client_id, message_index)[1]
    )

    # One initial lobby-list response is sent when a client connects.
    expected_responses = valid_messages_planned + 1

    try:
        async with websockets.connect(
            uri,
            open_timeout=connect_timeout_seconds,
            close_timeout=connect_timeout_seconds,
            ping_interval=None,
        ) as websocket:
            connected = True

            response_task = asyncio.create_task(
                collect_responses(
                    websocket=websocket,
                    expected_responses=expected_responses,
                    timeout_seconds=response_timeout_seconds,
                )
            )

            for message_index in range(messages_per_client):
                payload, is_malformed = build_message(
                    client_id,
                    message_index,
                )

                await websocket.send(payload)
                messages_sent += 1

                if is_malformed:
                    malformed_messages_sent += 1
                else:
                    valid_messages_sent += 1

                if delay_seconds > 0:
                    await asyncio.sleep(delay_seconds)

            completed = messages_sent == messages_per_client

            responses_received = await response_task
            response_complete = (
                responses_received == expected_responses
            )

    except Exception as exc:
        error = f"{type(exc).__name__}: {exc}"

    finally:
        if response_task is not None and not response_task.done():
            response_task.cancel()

        if response_task is not None:
            try:
                task_result = await response_task
                responses_received = max(
                    responses_received,
                    task_result,
                )
            except asyncio.CancelledError:
                pass

    return ClientResult(
        client_id=client_id,
        connected=connected,
        completed=completed,
        messages_sent=messages_sent,
        valid_messages_sent=valid_messages_sent,
        malformed_messages_sent=malformed_messages_sent,
        responses_received=responses_received,
        expected_responses=expected_responses,
        response_complete=response_complete,
        error=error,
    )


def percentage(numerator: int, denominator: int) -> float:
    if denominator == 0:
        return 0.0

    return numerator / denominator * 100.0


async def run_simulation(
    uri: str,
    clients: int,
    messages_per_client: int,
    delay_seconds: float,
    response_timeout_seconds: float,
    connect_timeout_seconds: float,
) -> None:
    started_at = time.perf_counter()

    tasks = [
        simulate_client(
            client_id=client_id,
            uri=uri,
            messages_per_client=messages_per_client,
            delay_seconds=delay_seconds,
            response_timeout_seconds=response_timeout_seconds,
            connect_timeout_seconds=connect_timeout_seconds,
        )
        for client_id in range(1, clients + 1)
    ]

    results = await asyncio.gather(*tasks)

    elapsed_seconds = time.perf_counter() - started_at

    connected_clients = sum(result.connected for result in results)
    completed_clients = sum(result.completed for result in results)
    failed_connections = clients - connected_clients

    messages_planned = clients * messages_per_client
    messages_sent = sum(result.messages_sent for result in results)
    messages_not_sent = messages_planned - messages_sent

    

    valid_messages_sent = sum(
        result.valid_messages_sent for result in results
    )
    malformed_messages_sent = sum(
        result.malformed_messages_sent for result in results
    )
    responses_received = sum(
        result.responses_received for result in results
    )

    response_complete_clients = sum(
        result.response_complete for result in results
    )

    expected_responses = sum(
        result.expected_responses for result in results
    )

    throughput = (
        messages_sent / elapsed_seconds
        if elapsed_seconds > 0
        else 0.0
    )

    print("\nClient simulation complete")
    print("=" * 48)
    print(f"WebSocket URI:                 {uri}")
    print(f"Clients attempted:             {clients}")
    print(f"Clients connected:             {connected_clients}")
    print(f"Clients failed to connect:     {failed_connections}")
    print(
        "Connection success rate:       "
        f"{percentage(connected_clients, clients):.2f}%"
    )
    print(f"Clients completed workload:    {completed_clients}")
    print(
        "Workload completion rate:      "
        f"{percentage(completed_clients, clients):.2f}%"
    )
    print(f"Messages per client:           {messages_per_client}")
    print(f"Messages planned:              {messages_planned}")
    print(f"Messages sent:                 {messages_sent}")
    print(f"Messages not sent:             {messages_not_sent}")
    print(
        "Send completion rate:          "
        f"{percentage(messages_sent, messages_planned):.2f}%"
    )
    print(f"Valid messages sent:           {valid_messages_sent}")
    print(
        "Malformed messages sent:       "
        f"{malformed_messages_sent}"
    )
    print(f"Expected server responses:     {expected_responses}")
    print(f"Server responses received:     {responses_received}")
    print(
        "Response completion rate:      "
        f"{percentage(responses_received, expected_responses):.2f}%"
    )
    print(
        "Clients receiving all replies: "
        f"{response_complete_clients}"
    )
    print(f"Elapsed time:                  {elapsed_seconds:.3f} seconds")
    print(f"End-to-end workload rate:      {throughput:.2f} messages/second")
    print("=" * 48)
    print(
        "Note: malformed-message counts are measured client-side. "
        "Confirm actual server drops using the server log."
    )

    failures = [result for result in results if result.error]

    if failures:
        print("\nClient errors:")

        for result in failures:
            print(f"- client {result.client_id}: {result.error}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Run concurrent WebSocket clients against the multiplayer "
            "game server."
        )
    )

    parser.add_argument(
        "--uri",
        default="ws://localhost:8080",
        help="WebSocket server URI. Default: ws://localhost:8080",
    )
    parser.add_argument(
        "--clients",
        type=int,
        default=50,
        help="Number of concurrent clients. Default: 50",
    )
    parser.add_argument(
        "--messages-per-client",
        type=int,
        default=20,
        help="Messages sent by each client. Default: 20",
    )
    parser.add_argument(
        "--delay",
        type=float,
        default=0.01,
        help=(
            "Delay in seconds between messages from each client. "
            "Default: 0.01"
        ),
    )
    parser.add_argument(
        "--response-timeout",
        type=float,
        default=15.0,
        help=(
            "Maximum time each client waits for expected server responses. "
            "Default: 15.0"
        ),
    )
    parser.add_argument(
        "--connect-timeout",
        type=float,
        default=10.0,
        help="WebSocket connection timeout in seconds. Default: 10",
    )

    return parser.parse_args()


def main() -> None:
    args = parse_args()

    if args.clients <= 0:
        raise SystemExit("--clients must be greater than 0")

    if args.messages_per_client <= 0:
        raise SystemExit("--messages-per-client must be greater than 0")

    if args.delay < 0:
        raise SystemExit("--delay cannot be negative")

    if args.response_timeout <= 0:
        raise SystemExit("--response-timeout must be greater than 0")

    if args.connect_timeout <= 0:
        raise SystemExit("--connect-timeout must be greater than 0")

    asyncio.run(
        run_simulation(
            uri=args.uri,
            clients=args.clients,
            messages_per_client=args.messages_per_client,
            delay_seconds=args.delay,
            response_timeout_seconds=args.response_timeout,
            connect_timeout_seconds=args.connect_timeout,
        )
    )


if __name__ == "__main__":
    main()