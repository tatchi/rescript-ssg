let modulePath = Utils.getFilepath()

type variant =
  | A
  | B(string)

type polyVariant = [#hello | #world]

type data = {
  string: string,
  int: int,
  float: float,
  variant: variant,
  polyVariant: polyVariant,
  bool: bool,
  option: option<string>,
}

@react.component
let make = (~data: option<data>) =>
  <div>
    {switch data {
    | None => React.null
    | Some({bool, string, int, float, variant, polyVariant, option}) => <>
        {bool->string_of_bool->React.string}
        {string->React.string}
        {int->Belt.Int.toString->React.string}
        {float->Belt.Float.toString->React.string}
        {switch variant {
        | A => "A"->React.string
        | B(_) => "B"->React.string
        }}
        {switch polyVariant {
        | #hello => "hello"->React.string
        | #world => "world"->React.string
        }}
        {switch option {
        | None => "None"->React.string
        | Some(s) => ("Some: " ++ s)->React.string
        }}
      </>
    }}
  </div>
