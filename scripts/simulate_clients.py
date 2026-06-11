#!/usr/bin/env python3

import argparse
import asyncio
from dataclasses import dataclass

import websockets


@dataclass
class ClientResult:
    client_id: int
    connected: bool
    messages_sent: int
    error: str = ""


DEFAULT_MESSAGES = [
    "CreateLobby",
    "StartJoinLobby",
    "StartGame:Host",
    "ResponseChoiceInput:Rock|p1_choice",
    "DeleteServer:now",
]


async def simulate_client(client_id: int, uri: str, delay_seconds: float) -> ClientResult:
    messages_sent = 0

    try:
        async with websockets.connect(uri) as websocket:
            for message in DEFAULT_MESSAGES:
                await websocket.send(message)
                messages_sent += 1

                # Give the server a small gap between messages so logs remain readable.
                if delay_seconds > 0:
                    await asyncio.sleep(delay_seconds)

            return ClientResult(
                client_id=client_id,
                connected=True,
                messages_sent=messages_sent,
            )

    except Exception as exc:
        return ClientResult(
            client_id=client_id,
            connected=False,
            messages_sent=messages_sent,
            error=str(exc),
        )


async def run_simulation(uri: str, clients: int, delay_seconds: float) -> None:
    tasks = [
        simulate_client(client_id=i + 1, uri=uri, delay_seconds=delay_seconds)
        for i in range(clients)
    ]

    results = await asyncio.gather(*tasks)

    successful = sum(1 for result in results if result.connected)
    failed = len(results) - successful
    total_messages = sum(result.messages_sent for result in results)

    print("Client simulation complete")
    print(f"WebSocket URI: {uri}")
    print(f"Clients attempted: {clients}")
    print(f"Clients connected: {successful}")
    print(f"Clients failed: {failed}")
    print(f"Messages sent: {total_messages}")

    if failed:
        print("\nFailures:")
        for result in results:
            if not result.connected:
                print(f"- client {result.client_id}: {result.error}")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Simulate multiple browser clients sending WebSocket protocol messages."
    )
    parser.add_argument(
        "--uri",
        default="ws://localhost:8080",
        help="WebSocket server URI. Default: ws://localhost:8080",
    )
    parser.add_argument(
        "--clients",
        type=int,
        default=5,
        help="Number of simulated clients. Default: 5",
    )
    parser.add_argument(
        "--delay",
        type=float,
        default=0.05,
        help="Delay in seconds between messages per client. Default: 0.05",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()

    if args.clients <= 0:
        raise SystemExit("--clients must be greater than 0")

    asyncio.run(
        run_simulation(
            uri=args.uri,
            clients=args.clients,
            delay_seconds=args.delay,
        )
    )


if __name__ == "__main__":
    main()