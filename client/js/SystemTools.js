class SystemTools {

    translateModifiers(event) {
        let modifiers = 0;
        if (event.shiftKey) modifiers |= 0x02000000; // Qt::ShiftModifier
        if (event.ctrlKey) modifiers |= 0x04000000; // Qt::ControlModifier
        if (event.altKey) modifiers |= 0x08000000; // Qt::AltModifier
        if (event.metaKey) modifiers |= 0x10000000; // Qt::MetaModifier
        return modifiers;
    }
}
export default  new SystemTools();