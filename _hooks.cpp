#include "_hooks.h"
#include <map>
#include <utility>
#include <string>

// Initialization
// Here (DllMain and DllLoad) all global variables are NOT initialized, use FONLINE_DLL_ENTRY instead
#if defined(FO_WINDOWS)
int __stdcall DllMain(void *module, unsigned long reason, void *reserved) { return 1; }
#elif defined(FO_LINUX)
void __attribute__((constructor)) DllLoad() {}
void __attribute__((destructor)) DllUnload() {}
#endif

map<pair<string, string>, ScriptHookHandler *> scriptHookTagNameHandlerRegistry;
map<pair<string, uint>, ScriptHookHandler *> scriptHookTagIdHandlerRegistry;

/**
 * @brief Registers callback for a specified named hook handler
 * 
 * @param tag Hook tag
 * @param name Handler name
 * @param callbackModule Callback module
 * @param callbackName Callback name
 */
void Global_Hook_Name(ScriptString &tag, ScriptString &name, ScriptString &callbackModule, ScriptString &callbackName)
{
    if (scriptHookTagNameHandlerRegistry.find({tag.c_str(), name.c_str()}) == scriptHookTagNameHandlerRegistry.end())
    {
        scriptHookTagNameHandlerRegistry[{tag.c_str(), name.c_str()}] = new ScriptHookHandler();
    }
    scriptHookTagNameHandlerRegistry[{tag.c_str(), name.c_str()}]->Push(callbackModule.c_str(), callbackName.c_str());
}

/**
 * @brief Registers callback for a specified indexed hook handler
 * 
 * @param tag Hook tag
 * @param index Handler index
 * @param callbackModule Callback module
 * @param callbackName Callback name
 */
void Global_Hook_Id(ScriptString &tag, uint index, ScriptString &callbackModule, ScriptString &callbackName)
{
    if (scriptHookTagIdHandlerRegistry.find({tag.c_str(), index}) == scriptHookTagIdHandlerRegistry.end())
    {
        scriptHookTagIdHandlerRegistry[{tag.c_str(), index}] = new ScriptHookHandler();
    }
    scriptHookTagIdHandlerRegistry[{tag.c_str(), index}]->Push(callbackModule.c_str(), callbackName.c_str());
}

/**
 * @brief Registers callback for a wildcard hook handler
 * 
 * @param tag Hook tag
 * @param module Callback module
 * @param name Callback name
 */
void Global_Hook(ScriptString &tag, ScriptString &module, ScriptString &name)
{
    Global_Hook_Id(tag, 0, module, name);
}

/**
 * @brief Dispatches an indexed hook handler
 * 
 * @param tag Hook tag
 * @param index Handler index
 */
void Global_Hook_Dispatch_Id(ScriptString &tag, uint index)
{
    const char *tagKey = tag.c_str();
    vector<ScriptGenericArgument *> args;

    if (scriptHookTagIdHandlerRegistry.find({tagKey, index}) != scriptHookTagIdHandlerRegistry.end())
    {
        scriptHookTagIdHandlerRegistry[{tagKey, index}]->Dispatch(args);
    }

    if (scriptHookTagIdHandlerRegistry.find({tagKey, 0}) != scriptHookTagIdHandlerRegistry.end())
    {
        scriptHookTagIdHandlerRegistry[{tagKey, 0}]->Dispatch(args);
    }
}

#define VARIADIC_ARGUMENTS_COUNT 10

#define VARIADIC_ARGUMENTS_DECLARATION \
    void *arg0, int argTypeId0,        \
        void *arg1, int argTypeId1,    \
        void *arg2, int argTypeId2,    \
        void *arg3, int argTypeId3,    \
        void *arg4, int argTypeId4,    \
        void *arg5, int argTypeId5,    \
        void *arg6, int argTypeId6,    \
        void *arg7, int argTypeId7,    \
        void *arg8, int argTypeId8,    \
        void *arg9, int argTypeId9

#define VARIADIC_ARGUMENTS_ASSIGNMENT(args)                      \
    args.push_back(new ScriptGenericArgument(arg0, argTypeId0)); \
    args.push_back(new ScriptGenericArgument(arg1, argTypeId1)); \
    args.push_back(new ScriptGenericArgument(arg2, argTypeId2)); \
    args.push_back(new ScriptGenericArgument(arg3, argTypeId3)); \
    args.push_back(new ScriptGenericArgument(arg4, argTypeId4)); \
    args.push_back(new ScriptGenericArgument(arg5, argTypeId5)); \
    args.push_back(new ScriptGenericArgument(arg6, argTypeId6)); \
    args.push_back(new ScriptGenericArgument(arg7, argTypeId7)); \
    args.push_back(new ScriptGenericArgument(arg8, argTypeId8)); \
    args.push_back(new ScriptGenericArgument(arg9, argTypeId9));

#define VARIADIC_ARGUMENTS_LIST \
    arg0, argTypeId0,           \
        arg1, argTypeId1,       \
        arg2, argTypeId2,       \
        arg3, argTypeId3,       \
        arg4, argTypeId4,       \
        arg5, argTypeId5,       \
        arg6, argTypeId6,       \
        arg7, argTypeId7,       \
        arg8, argTypeId8,       \
        arg9, argTypeId9

/**
 * @brief Dispatches an indexed hook handler with specified arguments
 * 
 * @param tag Hook tag
 * @param index Handler index
 */
void Global_Hook_Dispatch_Id_Arguments(ScriptString &tag, uint index, VARIADIC_ARGUMENTS_DECLARATION)
{
    const char *tagKey = tag.c_str();

    vector<ScriptGenericArgument *> args;
    VARIADIC_ARGUMENTS_ASSIGNMENT(args);

    if (scriptHookTagIdHandlerRegistry.find({tagKey, index}) != scriptHookTagIdHandlerRegistry.end())
    {
        scriptHookTagIdHandlerRegistry[{tagKey, index}]->Dispatch(args);
    }

    if (scriptHookTagIdHandlerRegistry.find({tagKey, 0}) != scriptHookTagIdHandlerRegistry.end())
    {
        scriptHookTagIdHandlerRegistry[{tagKey, 0}]->Dispatch(args);
    }
}

/**
 * @brief Dispatches a named hook handler
 * 
 * @param tag Hook tag
 * @param name Handler name
 */
void Global_Hook_Dispatch_Name(ScriptString &tag, ScriptString &name)
{
    const char *tagKey = tag.c_str();
    const char *nameKey = name.c_str();
    if (scriptHookTagNameHandlerRegistry.find({tagKey, 0}) == scriptHookTagNameHandlerRegistry.end())
        return;

    vector<ScriptGenericArgument *> args;
    if (scriptHookTagNameHandlerRegistry.find({tagKey, nameKey}) != scriptHookTagNameHandlerRegistry.end())
    {
        scriptHookTagNameHandlerRegistry[{tagKey, nameKey}]->Dispatch(args);
    }

    if (scriptHookTagIdHandlerRegistry.find({tagKey, 0}) != scriptHookTagIdHandlerRegistry.end())
    {
        scriptHookTagIdHandlerRegistry[{tagKey, 0}]->Dispatch(args);
    }
}

/**
 * @brief Dispatches a named hook handler with specified arguments
 * 
 * @param tag Hook tag
 * @param name Handler name
 */
void Global_Hook_Dispatch_Name_Arguments(ScriptString &tag, ScriptString &name, VARIADIC_ARGUMENTS_DECLARATION)
{
    const char *tagKey = tag.c_str();
    const char *nameKey = name.c_str();
    if (scriptHookTagNameHandlerRegistry.find({tagKey, 0}) == scriptHookTagNameHandlerRegistry.end())
        return;

    vector<ScriptGenericArgument *> args;
    VARIADIC_ARGUMENTS_ASSIGNMENT(args);

    if (scriptHookTagNameHandlerRegistry.find({tagKey, nameKey}) != scriptHookTagNameHandlerRegistry.end())
    {
        scriptHookTagNameHandlerRegistry[{tagKey, nameKey}]->Dispatch(args);
    }

    if (scriptHookTagIdHandlerRegistry.find({tagKey, 0}) != scriptHookTagIdHandlerRegistry.end())
    {
        scriptHookTagIdHandlerRegistry[{tagKey, 0}]->Dispatch(args);
    }
}

/**
 * @brief Dispatches only a wildcard hook handler
 * 
 * @param tag Hook tag
 */
void Global_Hook_Dispatch(ScriptString &tag)
{
    Global_Hook_Dispatch_Id(tag, 0);
}

/**
 * @brief Dispatches only a wildcard hook handler with specified arguments
 * 
 * @param tag Hook tag
 */
void Global_Hook_Dispatch_Arguments(ScriptString &tag, VARIADIC_ARGUMENTS_DECLARATION)
{
    Global_Hook_Dispatch_Id_Arguments(tag, 0, VARIADIC_ARGUMENTS_LIST);
}

#define ENTITY_HANDLERS_DECLARATION(ENTITY,INDEX,SUFFIX)                                                    \
    void ENTITY ## _Hook(ENTITY &instance, ScriptString &tag, ScriptString &module, ScriptString &function)     \
    {                                                                                                       \
        tag.append(SUFFIX);                                                                                 \
        Global_Hook_Id(tag, INDEX, module, function);                                                       \
    }                                                                                                       \
    void ENTITY ## _Dispatch(ENTITY &instance, ScriptString &tag)                                               \
    {                                                                                                       \
        tag.append(SUFFIX);                                                                                 \
        Global_Hook_Dispatch_Id(tag, INDEX);                                                                \
    }                                                                                                       \
    void ENTITY ## _Dispatch_Arguments(ENTITY &instance, ScriptString &tag, VARIADIC_ARGUMENTS_DECLARATION)     \
    {                                                                                                       \
        tag.append(SUFFIX);                                                                                 \
        Global_Hook_Dispatch_Id_Arguments(tag, INDEX, VARIADIC_ARGUMENTS_LIST);                             \
    }                                                                                                       

#define ENTITY_HANDLERS_REGISTRATION(ENTITY, CLASS)                                                                                                                          \
    ASEngine->RegisterObjectMethod(CLASS, "void Subscribe(string& tag, string& module, string& function)", asFUNCTION(ENTITY ## _Hook), asCALL_CDECL_OBJFIRST);                     \
    arguments = "";                                                                                                                                                         \
    ASEngine->RegisterObjectMethod(CLASS, "void Dispatch(string& tag)", asFUNCTION(ENTITY ## _Dispatch), asCALL_CDECL_OBJFIRST);                                             \
    for (uint i = 0; i < 10; i++)                                                                                                                                           \
    {                                                                                                                                                                       \
        arguments.append(", ?&inout");                                                                                                                                      \
        ASEngine->RegisterObjectMethod(CLASS, ("void Dispatch(string& tag" + arguments + ")").c_str(), asFUNCTION(ENTITY ## _Dispatch_Arguments), asCALL_CDECL_OBJFIRST);    \
    }

#ifdef __SERVER
ENTITY_HANDLERS_DECLARATION(Item,(instance.Id),"@Item")
ENTITY_HANDLERS_DECLARATION(Map,(instance.Data.MapId),"@Map")
ENTITY_HANDLERS_DECLARATION(Critter,(instance.Id),"@Critter")
#endif

#ifdef __CLIENT
ENTITY_HANDLERS_DECLARATION(Item,(instance.Id),"@Item")
ENTITY_HANDLERS_DECLARATION(CritterCl,(instance.Id),"@Critter")
#endif

///////////////////////////////////////////////////////////////////////////////
// Registration
///////////////////////////////////////////////////////////////////////////////

FONLINE_DLL_ENTRY(isCompiler)
{
    ASEngine->RegisterGlobalFunction("void Subscribe(string& tag, string& module, string& function)", asFUNCTION(Global_Hook), asCALL_CDECL);

    ASEngine->RegisterGlobalFunction("void Subscribe(string& tag, uint index, string& module, string& function)", asFUNCTION(Global_Hook_Id), asCALL_CDECL);
    ASEngine->RegisterGlobalFunction("void Subscribe(string& tag, string& name, string& module, string& function)", asFUNCTION(Global_Hook_Name), asCALL_CDECL);

    string arguments = "";
    ASEngine->RegisterGlobalFunction("void Dispatch(string& tag)", asFUNCTION(Global_Hook_Dispatch), asCALL_CDECL);
    for (uint i = 0; i < 10; i++)
    {
        arguments.append(", ?&inout");
        ASEngine->RegisterGlobalFunction(("void Dispatch(string& tag" + arguments + ")").c_str(), asFUNCTION(Global_Hook_Dispatch_Arguments), asCALL_CDECL);
    }

    arguments = "";
    ASEngine->RegisterGlobalFunction("void DispatchByIndex(string& tag, uint id)", asFUNCTION(Global_Hook_Dispatch_Id), asCALL_CDECL);
    for (uint i = 0; i < 10; i++)
    {
        arguments.append(", ?&inout");
        ASEngine->RegisterGlobalFunction(("void DispatchByIndex(string& tag, uint id" + arguments + ")").c_str(), asFUNCTION(Global_Hook_Dispatch_Id_Arguments), asCALL_CDECL);
    }

    arguments = "";
    ASEngine->RegisterGlobalFunction("void DispatchByName(string& tag, string& name)", asFUNCTION(Global_Hook_Dispatch_Name), asCALL_CDECL);
    for (uint i = 0; i < 10; i++)
    {
        arguments.append(", ?&inout");
        ASEngine->RegisterGlobalFunction(("void DispatchByName(string& tag, string& name" + arguments + ")").c_str(), asFUNCTION(Global_Hook_Dispatch_Name_Arguments), asCALL_CDECL);
    }

#ifdef __SERVER
    ENTITY_HANDLERS_REGISTRATION(Item,"Item")
    ENTITY_HANDLERS_REGISTRATION(Critter,"Critter")
    ENTITY_HANDLERS_REGISTRATION(Map,"Map")
#endif

#ifdef __CLIENT
    ENTITY_HANDLERS_REGISTRATION(Item,"ItemCl")
    ENTITY_HANDLERS_REGISTRATION(CritterCl,"CritterCl")
#endif
}
