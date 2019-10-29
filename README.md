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

# Examples

## Events
```angelscript
//
// logger.fos module
//
bool module_init() {
  Subscribe("Warning","logger","LogToFileStream");
}
void LogToFileStream(string message){
  // ...
}

//
// someothermodule.fos module random function body
//
Dispatch("Warning","Hello World!");
```
## Filters

```angelscript
//
// item_durability.fos module
//
bool module_init() {
  Subscribe("filter:item_cost","ItemCostFilter");
}
void ItemCostFilter(uint itemId, float& cost) {
  Item@ item = GetItem(itemId);
  if (null is item) return;
  
  if (item.Durability < 50) {
    cost *= 0.5;
  }
}
//
// economics.fos module
//
float get_item_cost(Item@ item) {
  float cost = float(item.Proto.Cost);
  Dispatch("filter:item_cost", item.Id, cost);
  
  return cost;
}
```

# Installation

Build and bind dynamic library somewhere at the top of script modules loading order:
- `#  pragma bindfunc  "void _hooks() -> ../bin/_hooks.server.dll SERVER"`
- `#  pragma bindfunc  "void _hooks() -> ../bin/_hooks.client.dll CLIENT"`
