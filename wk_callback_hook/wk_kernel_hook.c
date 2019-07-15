#include <ntddk.h>

DRIVER_INITIALIZE   DriverEntry;
DRIVER_UNLOAD       DriverUnload;

PVOID g_WdRegisteredCallback;

VOID
WdProcCallback(
    _In_opt_ PVOID CallbackContext,
    _In_opt_ PVOID Argument1,
    _In_opt_ PVOID Argument2
)
{
    UNREFERENCED_PARAMETER(CallbackContext);
    UNREFERENCED_PARAMETER(Argument1);
    UNREFERENCED_PARAMETER(Argument2);

    DbgPrintEx(DPFLTR_IHVDRIVER_ID,
               DPFLTR_INFO_LEVEL,
               __FUNCTION__ ": WOOT!?\n");
}

VOID DriverUnload(
    _In_    PDRIVER_OBJECT  DriverObject
)
{
    UNREFERENCED_PARAMETER(DriverObject);

    ExUnregisterCallback(g_WdRegisteredCallback);
}

NTSTATUS
DriverEntry(
    _In_    PDRIVER_OBJECT  DriverObject,
    _In_    PUNICODE_STRING RegistryPath
)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    DriverObject->DriverUnload = DriverUnload;

    PCALLBACK_OBJECT    pWdProcessNotifCb = NULL;
    UNICODE_STRING      WdObjName = RTL_CONSTANT_STRING(L"\\Callback\\WdProcessNotificationCallback");
    OBJECT_ATTRIBUTES   WdProcNotifObjAttr;
    RtlSecureZeroMemory(&WdProcNotifObjAttr, sizeof(OBJECT_ATTRIBUTES));
    WdProcNotifObjAttr.Length = sizeof(OBJECT_ATTRIBUTES);
    WdProcNotifObjAttr.ObjectName = &WdObjName;
    WdProcNotifObjAttr.Attributes = 0x210;

    if (!NT_SUCCESS(ExCreateCallback(&pWdProcessNotifCb, &WdProcNotifObjAttr, FALSE, FALSE))) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID,
                   DPFLTR_ERROR_LEVEL,
                   __FUNCTION__ ": Failed to obtain callback object!\n");

        return STATUS_NOT_FOUND;
    }

    g_WdRegisteredCallback = ExRegisterCallback(pWdProcessNotifCb, WdProcCallback, NULL);
    if (g_WdRegisteredCallback != NULL) {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID,
                   DPFLTR_INFO_LEVEL,
                   __FUNCTION__ ": Seems ok!\n");

    } else {
        DbgPrintEx(DPFLTR_IHVDRIVER_ID,
                   DPFLTR_INFO_LEVEL,
                   __FUNCTION__ ": Hmmm, something wrong!\n");

    }

    return STATUS_SUCCESS;
}