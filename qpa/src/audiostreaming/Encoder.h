void wavify(QByteArray &bytes, int numberOfChannels, int sampleRate)
{
    // qInfo() << "wavifying" << numberOfChannels << sampleRate;
    QByteArray header(44, 0); 
    //setUint16(20, 1, true);  true means le, do I need to flip the my bytes?
    uint8_t *uint8Bytes = reinterpret_cast<uint8_t *>(header.data());
    uint16_t *uint16Bytes = reinterpret_cast<uint16_t *>(header.data());
    uint32_t *uint32Bytes = reinterpret_cast<uint32_t *>(header.data());

    uint8Bytes[0] = 'R'; // d.setUint8(0, "R".charCodeAt(0));
    uint8Bytes[1] = 'I'; // d.setUint8(1, "I".charCodeAt(0));
    uint8Bytes[2] = 'F'; // d.setUint8(2, "F".charCodeAt(0));
    uint8Bytes[3] = 'F'; // d.setUint8(3, "F".charCodeAt(0));

    uint32Bytes[1] = bytes.length() + 44; // d.setUint32(4, data.byteLength / 2 + 44, true);

    uint8Bytes[8] = 'W';  // d.setUint8(8, "W".charCodeAt(0));
    uint8Bytes[9] = 'A';  // d.setUint8(9, "A".charCodeAt(0));
    uint8Bytes[10] = 'V'; // d.setUint8(10, "V".charCodeAt(0));
    uint8Bytes[11] = 'E'; // d.setUint8(11, "E".charCodeAt(0));

    uint8Bytes[12] = 'f'; // d.setUint8(12, "f".charCodeAt(0));
    uint8Bytes[13] = 'm'; // d.setUint8(13, "m".charCodeAt(0));
    uint8Bytes[14] = 't'; // d.setUint8(14, "t".charCodeAt(0));
    uint8Bytes[15] = ' '; // d.setUint8(15, " ".charCodeAt(0));

    uint32Bytes[4] = 16;                                      // d.setUint32(16, 16, true);
    uint16Bytes[10] = 1;                                      // d.setUint16(20, 1, true);
    uint16Bytes[11] = numberOfChannels;                       // d.setUint16(22, numberOfChannels, true);
    uint32Bytes[6] = sampleRate;                              // d.setUint32(24, sampleRate, true);
    uint32Bytes[7] = (sampleRate * numberOfChannels * 2) / 8; // d.setUint32(28, sampleRate * 1 * 2);
    uint16Bytes[16] = numberOfChannels * 2;                   // d.setUint16(32, numberOfChannels * 2);
    uint16Bytes[17] = 16;                                     // d.setUint16(34, 16, true);

    uint8Bytes[36] = 'd';             // d.setUint8(36, "d".charCodeAt(0));
    uint8Bytes[37] = 'a';             // d.setUint8(37, "a".charCodeAt(0));
    uint8Bytes[38] = 't';             // d.setUint8(38, "t".charCodeAt(0));
    uint8Bytes[39] = 'a';             // d.setUint8(39, "a".charCodeAt(0));
    uint32Bytes[10] = bytes.length(); // d.setUint32(40, data.byteLength, true);
    // qInfo() << header;

    // qInfo() << "stuff done";
    bytes.prepend(header);
}