# FOnline SDK Event Hooks

Library adds event-like functions and methods `Subscribe` and `Dispatch` to subscribe a set of script callbacks to a named event and call it afterwards (up to 10 optional arguments supported).

## Global:
- `void Subscribe(string& tag, string& module, string& function)`
- `void Subscribe(string& tag, uint index, string& module, string& function)`
- `void Subscribe(string& tag, string& name, string& module, string& function)`
- `void Dispatch(string& tag)`
- `void Dispatch(string& tag, ...?&inout)`
- `void DispatchByIndex(string& tag, uint index)`
- `void DispatchByIndex(string& tag, uint index, ...?&inout)`
- `void DispatchByName(string& tag, string& name)`
- `void DispatchByName(string& tag, string& name, ...?&inout)`

## Critter & CritterCl:
- `void Critter::Subscribe(string& tag, string& module, string& function)`
- `void Critter::Dispatch(string& tag)`
- `void Critter::Dispatch(string& tag, ...?&inout)`

## Item & ItemCl:
- `void Item::Subscribe(string& tag, string& module, string& function)`
- `void Item::Dispatch(string& tag)`
- `void Item::Dispatch(string& tag, ...?&inout)`

## Map:
- `void Map::Subscribe(string& tag, string& module, string& function)`
- `void Map::Dispatch(string& tag)`
- `void Map::Dispatch(string& tag, ...?&inout)`

## How to install

Compile and bind somewhere at the top of script modules loading order:
- `#  pragma bindfunc  "void _events() -> ../bin/_events.server.dll SERVER"`
- `#  pragma bindfunc  "void _events() -> ../bin/_events.client.dll SERVER"`
