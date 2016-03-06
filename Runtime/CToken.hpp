#ifndef __PSHAG_CTOKEN_HPP__
#define __PSHAG_CTOKEN_HPP__

#include <memory>
#include "IObj.hpp"
#include "RetroTypes.hpp"
#include "IVParamObj.hpp"
#include "IObjectStore.hpp"
#include "IFactory.hpp"

namespace urde
{
class IObjectStore;
class IObj;

/** Shared data-structure for CToken references, analogous to std::shared_ptr */
class CObjectReference
{
    friend class CToken;
    u16 x0_refCount = 0;
    u16 x2_lockCount = 0;
    bool x3_loading = false; /* Rightmost bit of lockCount */
    SObjectTag x4_objTag;
    IObjectStore* xC_objectStore = nullptr;
    IObj* x10_object = nullptr;
    CVParamTransfer x14_params;
public:
    CObjectReference(IObjectStore& objStore, std::unique_ptr<IObj>&& obj,
                     const SObjectTag& objTag, CVParamTransfer buildParams)
    : x4_objTag(objTag), xC_objectStore(&objStore),
      x10_object(obj.release()), x14_params(buildParams) {}
    CObjectReference(std::unique_ptr<IObj>&& obj)
    : x10_object(obj.release()) {}

    /** Indicates an asynchronous load transaction has been submitted and is not yet finished */
    bool IsLoading() const {return x3_loading;}

    /** Decrements 2nd ref-count, performing unload or async-load-cancel if 0 reached */
    void Unlock()
    {
        --x2_lockCount;
        if (x2_lockCount)
            return;
        if (x10_object && xC_objectStore)
            Unload();
        else if (IsLoading())
            CancelLoad();
    }

    /** Increments 2nd ref-count, performing async-factory-load if needed */
    void Lock()
    {
        ++x2_lockCount;
        if (!x10_object && !x3_loading)
        {
            IFactory& fac = xC_objectStore->GetFactory();
            fac.BuildAsync(x4_objTag, x14_params, &x10_object);
            x3_loading = true;
        }
    }

    /** Mechanism by which CToken decrements 1st ref-count, indicating CToken invalidation or reset.
     *  Reaching 0 indicates the CToken should delete the CObjectReference */
    u16 RemoveReference()
    {
        --x0_refCount;
        if (x0_refCount == 0)
        {
            if (x10_object)
                Unload();
            if (IsLoading())
                CancelLoad();
            xC_objectStore->ObjectUnreferenced(x4_objTag);
        }
        return x0_refCount;
    }
    void CancelLoad()
    {
        if (xC_objectStore && IsLoading())
        {
            xC_objectStore->GetFactory().CancelBuild(x4_objTag);
            x3_loading = false;
        }
    }

    /** Pointer-synchronized object-destructor, another building Lock cycle may be performed after */
    void Unload()
    {
        delete x10_object;
        x10_object = nullptr;
        x3_loading = false;
    }

    /** Synchronous object-fetch, guaranteed to return complete object on-demand, blocking build if not ready */
    IObj* GetObject()
    {
        if (!x10_object)
        {
            IFactory& factory = xC_objectStore->GetFactory();
            x10_object = factory.Build(x4_objTag, x14_params).release();
        }
        x3_loading = false;
        return x10_object;
    }
    const SObjectTag& GetObjectTag() const
    {
        return x4_objTag;
    }
    ~CObjectReference()
    {
        if (x10_object)
            delete x10_object;
        else if (x3_loading)
            xC_objectStore->GetFactory().CancelBuild(x4_objTag);
    }
};

/** Counted meta-object, reference-counting against a shared CObjectReference
 *  This class is analogous to std::shared_ptr and C++11 rvalues have been implemented accordingly
 *  (default/empty constructor, move constructor/assign) */
class CToken
{
    CObjectReference* x0_objRef = nullptr;
    bool x4_lockHeld = false;
public:
    /* Added to test for non-null state */
    operator bool() const {return x0_objRef != nullptr;}
    void Unlock()
    {
        if (x0_objRef && x4_lockHeld)
        {
            x0_objRef->Unlock();
            x4_lockHeld = false;
        }
    }
    void Lock()
    {
        if (x0_objRef && !x4_lockHeld)
        {
            x0_objRef->Lock();
            x4_lockHeld = true;
        }
    }
    void RemoveRef()
    {
        if (x0_objRef && x0_objRef->RemoveReference() == 0)
        {
            delete x0_objRef;
            x0_objRef = nullptr;
        }
    }
    IObj* GetObj()
    {
        if (!x0_objRef)
            return nullptr;
        Lock();
        return x0_objRef->GetObject();
    }
    CToken& operator=(const CToken& other)
    {
        Unlock();
        RemoveRef();
        x0_objRef = other.x0_objRef;
        if (x0_objRef)
        {
            ++x0_objRef->x0_refCount;
            if (other.x4_lockHeld)
                Lock();
        }
        return *this;
    }
    CToken& operator=(CToken&& other)
    {
        Unlock();
        RemoveRef();
        x0_objRef = other.x0_objRef;
        other.x0_objRef = nullptr;
        x4_lockHeld = other.x4_lockHeld;
        other.x4_lockHeld = false;
        return *this;
    }
    CToken() = default;
    CToken(const CToken& other)
    : x0_objRef(other.x0_objRef)
    {
        ++x0_objRef->x0_refCount;
    }
    CToken(CToken&& other)
    : x0_objRef(other.x0_objRef), x4_lockHeld(other.x4_lockHeld)
    {
        other.x0_objRef = nullptr;
        other.x4_lockHeld = false;
    }
    CToken(IObj* obj)
    {
        x0_objRef = new CObjectReference(std::unique_ptr<IObj>(obj));
        ++x0_objRef->x0_refCount;
        Lock();
    }
    CToken(CObjectReference* obj)
    {
        x0_objRef = obj;
        ++x0_objRef->x0_refCount;
    }
    const SObjectTag* GetObjectTag() const
    {
        if (!x0_objRef)
            return nullptr;
        return &x0_objRef->GetObjectTag();
    }
    ~CToken()
    {
        if (x0_objRef)
        {
            if (x4_lockHeld)
                x0_objRef->Unlock();
            RemoveRef();
        }
    }
};

template <class T>
class TToken : public CToken
{
public:
    static std::unique_ptr<TObjOwnerDerivedFromIObj<T>> GetIObjObjectFor(std::unique_ptr<T>&& obj)
    {
        return TObjOwnerDerivedFromIObj<T>::GetNewDerivedObject(std::move(obj));
    }
    TToken() = default;
    TToken(const CToken& other) : CToken(other) {}
    TToken(CToken&& other) : CToken(std::move(other)) {}
    TToken(T* obj)
    : CToken(GetIObjObjectFor(std::unique_ptr<T>(obj))) {}
    TToken& operator=(T* obj) {*this = CToken(GetIObjObjectFor(obj)); return this;}
    T* GetObj()
    {
        if (CToken::GetObj())
            return static_cast<TObjOwnerDerivedFromIObj<T>*>(CToken::GetObj())->GetObj();
        return nullptr;
    }
    T* operator->() {return GetObj();}
};

template <class T>
class TLockedToken : public TToken<T>
{
    T* m_obj;
public:
    TLockedToken() : m_obj(nullptr) {}
    TLockedToken(const CToken& other) : TToken<T>(other) {m_obj = TToken<T>::GetObj();}
    TLockedToken(CToken&& other) : TToken<T>(std::move(other)) {m_obj = TToken<T>::GetObj();}
    T* GetObj() {return m_obj;}
    T* operator->() {return m_obj;}
};

}

#endif // __PSHAG_CTOKEN_HPP__
