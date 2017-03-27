// TODO: implement "DLL" import hooking here, eventually

// do this in a similar way to how we do detouring
// (i.e. have one CHookedFunc per imported function, and potentially multiple hooks)

// it's actually probably stupid-easy to do the hooking (on Linux AND Windows):
// - use address operator on function that's imported
// - you now have the addr of the uintptr_t where the actual addr of the func gets put
// - so, save off the old value for now
// - then overwrite it with the addr of the "main" hook (which calls all of the sub-hook(s))
// - when done, restore the old value back into place

// we'll need to be careful to note that hooks only take effect for ONE library's "point of view"
// so the hook declaration will need to specify which library they want to hook the func import for
