module.exports = grammar({
  name: 'socialgaming',

  extras: $ => [
    /\s/,
    $.comment,
  ],

  rules: {

    // Start symbol.
    // Each parsed input must define a game.
    game: $ => seq(
      field('configuration', $.configuration),
      field('constants',     $.constants),
      field('variables',     $.variables),
      field('per_player',    $.per_player),
      field('per_audience',  $.per_audience),
      field('rules',         $.rules)
    ),

    configuration: $ => seq(
      'configuration', '{',
        'name:',         field('name', $.quoted_string),
        'player range:', field('player_range', $.number_range),
        'audience:',     field('has_audience', $.boolean),
        'setup:', '{',
          repeat($.setup_rule),
        '}',
      '}'
    ),

    setup_rule: $ => seq(
      field('name', $.identifier), '{',
        'kind:',   field('kind', choice('boolean', 'integer', 'string', 'enum', 'question-answer', 'multiple-choice', 'json')),
        'prompt:', field('prompt', $.quoted_string),

        optional(seq('range:',   field('range', $.number_range))),
        optional(seq('choices:', field('choices', wrappedDelimitedList('{', $.enum_description, ',', '}')))),
        optional(seq('default:', field('default', $.expression))),
      '}'
    ),

    enum_description: $ => seq(
      field('name', $.identifier),
      ':',
      field('description', $.quoted_string),
    ),

    constants: $ => seq(
      'constants',
      field('map', $.value_map)
    ),

    variables: $ => seq(
      'variables',
      field('map', $.value_map)
    ),

    per_player: $ => seq(
      'per-player',
      field('map', $.value_map)
    ),

    per_audience: $ => seq(
      'per-audience',
      field('map', $.value_map)
    ),

    rules: $ => seq(
      'rules', field('body', $.body)
    ),

    /////////////////////////////////////////////////////////////////////////
    // General rule infrastructure
    /////////////////////////////////////////////////////////////////////////

    rule: $ => choice(
      // Grouping
      $.body,

      // Iteration and parallelism
      $.for,
      $.loop,
      $.parallel_for,
      $.in_parallel,

      // Selection
      $.match,

      // Lists
      $.extend,
      $.reverse,
      $.shuffle,
      $.sort,
      $.deal,
      $.discard,

      $.assignment,

      $.timer,

      // Input
      $.input_choice,
      $.input_text,
      $.input_vote,
      $.input_range,

      // Output
      $.message,
      $.scores,
    ),

    /////////////////////////////////////////////////////////////////////////
    // Iteration and parallelism rules
    /////////////////////////////////////////////////////////////////////////

    for: $ => seq(
      'for',
      field('element', $.identifier),
      'in',
      field('list', $.expression),
      field('body', $.body)
    ),

    loop: $ => seq(
      'while',
      field('condition', $.expression),
      field('body', $.body)
    ),

    parallel_for: $ => seq(
      'parallel for',
      field('element', $.identifier),
      'in',
      field('list', $.expression),
      field('body', $.body)
    ),

    in_parallel: $ => seq(
      'in parallel',
      field('body', $.body)
    ),

    /////////////////////////////////////////////////////////////////////////
    // Selection rules
    /////////////////////////////////////////////////////////////////////////

    match: $ => seq(
      'match',
      field('target', $.expression),
      '{',
        repeat1($.match_entry),
      '}'
    ),

    match_entry: $ => seq(
      field('guard', $.expression),
      '=>',
      field('body', $.body)
    ),

    /////////////////////////////////////////////////////////////////////////
    // List manipulation rules
    /////////////////////////////////////////////////////////////////////////

    extend: $ => seq(
      'extend',
      field('target', $.qualified_identifier),
      'with',
      field('value', $.expression),
      ';'
    ),

    reverse: $ => seq(
      'reverse',
      field('target', $.qualified_identifier),
      ';'
    ),

    shuffle: $ => seq(
      'shuffle',
      field('target', $.qualified_identifier),
      ';'
    ),

    sort: $ => seq(
      'sort',
      field('target', $.qualified_identifier),
      optional(seq('by', field('key', $.qualified_identifier))),
      ';'
    ),

    deal: $ => seq(
      'deal',
      field('count', $.expression), // Number
      'to',
      field('targets', $.player_set),
      'from',
      field('source', $.expression),
      ';'
    ),

    discard: $ => seq(
      'discard',
      field('count', $.expression), // Number
      'from',
      field('source', $.qualified_identifier),
      ';'
    ),

    /////////////////////////////////////////////////////////////////////////
    // Assignment rules
    /////////////////////////////////////////////////////////////////////////

    assignment: $ => seq(
      field('target', $.qualified_identifier),
      '<-',
      field('value', $.expression),
      ';'
    ),

    /////////////////////////////////////////////////////////////////////////
    // Timing rules
    /////////////////////////////////////////////////////////////////////////

    timer: $ => seq(
      'time',
      'for',
      field('duration', $.expression), // Number
      field('mode', optional(choice('at most', 'exactly'))),
      optional(seq('in', field('flag', $.qualified_identifier))),
      field('body', $.body)
    ),

    /////////////////////////////////////////////////////////////////////////
    // Input rules
    /////////////////////////////////////////////////////////////////////////

    input_choice: $ => seq(
      'input choice',
      'to',
      field('player', $.qualified_identifier),
      '{',
      'prompt:',
      field('prompt', $.expression), // String
      'choices:',
      field('choices', $.expression), // List
      'target:',
      field('target', $.qualified_identifier),
      optional(seq(
        'timeout:',
        field('timeout', $.expression) // Number
      )),
      '}'
    ),

    input_text: $ => seq(
      'input text',
      'to',
      field('player', $.qualified_identifier),
      '{',
      'prompt:',
      field('prompt', $.expression), // String
      'target:',
      field('target', $.qualified_identifier),
      optional(seq(
        'timeout:',
        field('timeout', $.expression) // Number
      )),
      '}'
    ),

    input_range: $ => seq(
      'input range',
      'to',
      field('player', $.qualified_identifier),
      '{',
      'prompt:',
      field('prompt', $.expression), // String
      'range:',
      field('range', $.number_range),
      field('low_label',  optional(seq('low:',  $.expression))), // String
      field('high_label', optional(seq('high:', $.expression))), // String
      'target:',
      field('target', $.qualified_identifier),
      optional(seq(
        'timeout:',
        field('timeout', $.expression) // Number
      )),
      '}'
    ),

    input_vote: $ => seq(
      'input vote',
      'to',
      field('players', $.player_set),
      field('anonymously', optional('anonymously')),
      '{',
      'prompt:',
      field('prompt', $.expression), // String
      'choices:',
      field('choices', $.expression), // List
      'target:',
      field('target', $.qualified_identifier), // Stores a map from choices to counts
      optional(seq(
        'timeout:',
        field('timeout', $.expression) // Number
      )),
      '}'
    ),

    /////////////////////////////////////////////////////////////////////////
    // Output rules
    /////////////////////////////////////////////////////////////////////////

    message: $ => seq(
      'message',
      field('players', $.player_set),
      field('content', $.expression), // String
      ';'
    ),

    scores: $ => seq(
      'scores',
      field('keys', $.list_literal),
      ';'
    ),

    /////////////////////////////////////////////////////////////////////////
    // Expressions
    /////////////////////////////////////////////////////////////////////////

    expression: $ => choice(
      $.boolean,
      $.number,
      $.quoted_string,
      $.list_literal,
      $.identifier,
      $.value_map,

      seq('(', field('operand', $.expression), ')'),

      prec.left(1, binaryExpression($.expression, '||', $.expression)),

      prec.left(2, binaryExpression($.expression, '&&', $.expression)),

      prec.left(3, binaryExpression($.expression, '=', $.expression)),
      prec.left(3, binaryExpression($.expression, '!=', $.expression)),

      prec.left(4, binaryExpression($.expression,
                                    choice('<', '<=', '>', '>='),
                                    $.expression)),

      prec.left(5, binaryExpression($.expression, '+', $.expression)),
      prec.left(5, binaryExpression($.expression, '-', $.expression)),

      prec.left(6, binaryExpression($.expression, '*', $.expression)),
      prec.left(6, binaryExpression($.expression, '/', $.expression)),
      prec.left(6, binaryExpression($.expression, '%', $.expression)),

      prec.left(7, seq('!', field('operand', $.expression))),

      // TODO: This allows expressing what we want cleanly, but it
      // also enables the expression of ideas that we may not want to be able
      // to express.
      prec.left(8, seq($.expression, '.', field('identifier', $.identifier))),

      prec.left(9, seq($.expression, '.', field('builtin', $.builtin), $.argument_list))
    ),

    builtin: $ => choice(
      // Integer builtins
      'upfrom',

      // List builtins
      'size',
      'contains',
      'collect'
    ),

    argument_list: $ => seq(
      '(',
        field('arguments', optional($.expression_list)),
      ')'
    ),

    /////////////////////////////////////////////////////////////////////////
    // Core reusable primitives
    /////////////////////////////////////////////////////////////////////////

    number: $ => /-?\d+/,

    boolean: $ => choice('true', 'false'),

    quoted_string: $ => seq(
      '"',
      field('contents', repeat(choice($.string_interpolation, $.string_text))),
      '"'
    ),

    // It is possible to use external lexer functions to drive detailed
    // matching of string content with escapes as rich as in Python. The
    // complexity of that is not worth it for our use cases, so we'll support
    // a simpler subset of string functionality instead.
    string_text: $ => /([^"{}]|\{\{|\}\})+/,

    string_interpolation: $ => seq(
      '{',
      field('expression', $.expression),
      '}'
    ),

    number_range: $ => seq(
      '(',
      $.number,
      ',',
      $.number,
      ')'
    ),

    identifier: $ => /[a-zA-Z_][a-zA-Z0-9_]*/,

    expression_list: $ => nonemptyDelimitedList($.expression, ','),

    list_literal: $ => seq(
      '[',
        field('elements', optional($.expression_list)),
      ']'
    ),

    map_entry: $ => seq(
      field('key', $.identifier),
      ':',
      field('value', $.expression)
    ),
    value_map: $ => wrappedDelimitedList('{', $.map_entry, ',', '}'),

    qualified_identifier: $ => prec.left(seq(
      repeat(seq($.identifier, '.')),
      $.identifier
    )),

    player_set: $ => choice('all', $.expression), // List

    body: $ => seq(
      '{',
      repeat($.rule),
      '}'
    ),

    comment: $ => token(seq('//', /.*/)),
  }
});

// NOTE: If the same element type and delimiter occur, then they should be
// factored out into a rule (like expression_list). Otherwise, tree-sitter
// collapses them onto a single name implicitly and creates less understandable
// parse trees.

function nonemptyDelimitedList(element, delimiter) {
  return seq(element, repeat(seq(delimiter, element)), optional(delimiter));
}

function wrappedDelimitedList(begin, element, delimiter, end) {
  return seq(
    begin,
      optional(nonemptyDelimitedList(element, delimiter)),
    end
  );
}

function binaryExpression(lhs, operator, rhs) {
  return seq(
    field('lhs', lhs),
    operator,
    field('rhs', rhs)
  );
}
