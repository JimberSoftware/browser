function changeCursor(cursor) {
    switch (parseInt(cursor)) {
        case 0: //Qt::ArrowCursor
            document.body.style.cursor = "default";
            break;
        case 2: //Qt::CrossCursor
            document.body.style.cursor = "crosshair";
            break;
        case 3: //Qt::WaitCursor
            document.body.style.cursor = "wait";
            break;
        case 4: //Qt::IBeamCursor
            document.body.style.cursor = "text";
            break;
        case 5: //Qt::SizeVerCursor
            document.body.style.cursor = "n-resize";
            break;
        case 6: //Qt::SizeHorCursorkeyHandler
            document.body.style.cursor = "e-resize";
            break;
        case 7: //Qt::SizeBDiagCursor
            document.body.style.cursor = "ne-resize";
            break;
        case 8: //Qt::SizeFDiagCursor
            document.body.style.cursor = "se-resize";
            break;
        case 9: //Qt::SizeAllCursor
            document.body.style.cursor = "move";
            break;
        case 10: //Qt::BlankCursor
            document.body.style.cursor = "none";
            break;
        case 11: //Qt::SplitVCursor
            document.body.style.cursor = "row-resize";
            break;
        case 12: //Qt::SplitHCursor
            document.body.style.cursor = "col-resize";
            break;
        case 13: //Qt::PointingHandCursor
            document.body.style.cursor = "pointer";
            break;
        case 14: //Qt::ForbiddenCursor
            document.body.style.cursor = "not-allowed";
            break;
        case 15: //Qt::WhatsThisCursor
            document.body.style.cursor = "help";
            break;
        case 16: //Qt::Busy    alert('key');Cursor
            document.body.style.cursor = "progress";
            break;
        case 17: //Qt::OpenHandCursor
            document.body.style.cursor = "pointer";
            break;
        case 18: //Qt::ClosedHandCursor
            document.body.style.cursor = "pointer";
            break;
        case 19: //Qt::DragMoveCursor
            document.body.style.cursor = "pointer";
            break;
        case 20: //Qt::DragCopyCursor
            document.body.style.cursor = "pointer";
            break;
        case 21: //Qt::DragLinkCursor
            document.body.style.cursor = "pointer";
            break;
        default:
            document.body.style.cursor = "default";
    }
}

export default changeCursor;