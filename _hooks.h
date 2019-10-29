#ifndef __HOOKS_DLL__
#define __HOOKS_DLL__

// Script constants
#define SKIP_PRAGMAS
#include "../scripts/_defines.fos"

// Disable macro redefinition warning
#pragma warning(push)
#pragma warning(disable : 4005)
#include "fonline.h"
#pragma warning(pop)

/**
 * @brief AngelScript variable parameter wrapper
 * 
 */
struct ScriptGenericArgument
{
    void *Address;
    int TypeId = 0;

    /**
     * @brief Construct a new Script Generic Argument object
     * 
     * @param address 
     * @param typeId 
     */
    ScriptGenericArgument(void *address, int typeId)
    {
        this->Address = address;
        this->TypeId = typeId;
    }

    /**
     * @brief Injects stored argument into asIScriptContext
     * 
     * @param ctx Target context
     * @param id  Target argument index
     * @param flags Argument flags
     * @return int 
     */
    int AddToContext(asIScriptContext *ctx, int id, asDWORD flags = 0)
    {
        if (asTYPEID_VOID == this->TypeId)
        {
            return asINVALID_ARG;
        }

        int r = asSUCCESS;

        if (this->TypeId & asTYPEID_OBJHANDLE || this->TypeId & asTYPEID_MASK_OBJECT)
        {
            r = ctx->SetArgObject(id, this->Address);
        }
        else if ((flags != 0 && flags != asTM_CONST) || this->TypeId & asTYPEID_HANDLETOCONST)
        {
            r = ctx->SetArgAddress(id, this->Address);
        }
        else
            switch (this->TypeId)
            {
            case asTYPEID_BOOL:
            {
                bool *arg = static_cast<bool *>(this->Address);
                r = ctx->SetArgByte(id, *arg);
                break;
            }
            case asTYPEID_INT8:
            case asTYPEID_INT16:
            case asTYPEID_INT32:
            case asTYPEID_INT64:
            {
                int *arg = static_cast<int *>(this->Address);
                r = ctx->SetArgDWord(id, *arg);
                break;
            }
            case asTYPEID_UINT8:
            case asTYPEID_UINT16:
            case asTYPEID_UINT32:
            case asTYPEID_UINT64:
            {
                uint *arg = static_cast<uint *>(this->Address);
                r = ctx->SetArgDWord(id, *arg);
                break;
            }
            case asTYPEID_FLOAT:
            {
                float *arg = static_cast<float *>(this->Address);
                r = ctx->SetArgFloat(id, *arg);
                break;
            }
            case asTYPEID_DOUBLE:
            {
                double *arg = static_cast<double *>(this->Address);
                r = ctx->SetArgDouble(id, *arg);
                break;
            }
            default:
            {
                r = ctx->SetArgVarType(id, this->Address, this->TypeId);
            }
            }

        return r;
    }
};

/**
 * @brief AngelScript callback function wrapper
 * 
 */
struct ScriptHookCallback
{
    string Module;
    string Name;

    asIScriptContext *Context;
    asIScriptFunction *Function;
    uint ArgumentsCount;
    vector<asDWORD> ArgumentFlags;

    /**
     * @brief Construct a new Script Hook Callback object
     * 
     * @param module 
     * @param name 
     */
    ScriptHookCallback(const char *module, const char *name)
    {
        this->Module = module;
        this->Name = name;

        this->Context = ASEngine->CreateContext();
        this->Function = ASEngine->GetModule(this->Module.c_str())->GetFunctionByName(this->Name.c_str());

        this->ArgumentsCount = this->Function->GetParamCount();
        for (uint i = 0; i < this->ArgumentsCount; i++)
        {
            int typeId;
            asDWORD flags = 0;
            const char *name = 0;
            const char *defaultArg = 0;
            this->Function->GetParam(i, &typeId, &flags, &name, &defaultArg);
            this->ArgumentFlags.push_back(flags);
        }
    }

    /**
     * @brief Calls the stored callback with arguments
     * 
     * @param arguments 
     */
    void Dispatch(vector<ScriptGenericArgument *> arguments)
    {
        //this->Context = ASEngine->CreateContext();
        uint argumentsCount = arguments.size();

        if (argumentsCount != this->ArgumentsCount)
        {
            Log(("Warning: Hook callback " + this->Name + "@" + this->Module + " requires " + std::to_string(this->ArgumentsCount) + " arguments, but only " + std::to_string(arguments.size()) + " are given.\n").c_str());
            return;
        }

        this->Context->Prepare(this->Function);
        for (uint i = 0; i < this->ArgumentsCount && i < argumentsCount; i++)
        {
            if (asSUCCESS != arguments[i]->AddToContext(this->Context, i, this->ArgumentFlags[i]))
            {
                //this->Context->Release();
                this->Context->Unprepare();
                return;
            }
        }

        int result = this->Context->Execute();
        if (asEXECUTION_FINISHED == result)
        {
            // Do something about the return value
            asBYTE ret = this->Context->GetReturnByte();
        }
        //this->Context->Release();
        this->Context->Unprepare();
        return;
    }
};

/**
 * @brief Collection of callbacks
 * 
 */
struct ScriptHookHandler
{
    vector<ScriptHookCallback *> Callbacks;

    /**
     * @brief Registers callback for current hook handler
     * 
     * @param module 
     * @param name 
     */
    void Push(const char *module, const char *name)
    {
        const char *ownerModule = ScriptGetActiveContext()->GetFunction(0)->GetModuleName();
        if (NULL == module)
        {
            module = ownerModule;
        }

        Log(("Subscribed <" + std::string(name) + "@" + std::string(module) + "> at <" + std::string(ownerModule) + ">.\n").c_str());
        this->Callbacks.push_back(new ScriptHookCallback(module, name));
    }

    /**
     * @brief Sequentially calls all registered callbacks for current handler
     * 
     * @param arguments 
     */
    void Dispatch(vector<ScriptGenericArgument *> arguments)
    {
        for (uint i = 0, l = this->Callbacks.size(); i < l; i++)
        {
            this->Callbacks[i]->Dispatch(arguments);
        }
    }
};

#endif // __HOOKS_DLL__
