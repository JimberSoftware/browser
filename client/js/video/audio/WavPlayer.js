(function (f) { if (typeof exports === "object" && typeof module !== "undefined") { module.exports = f() } else if (typeof define === "function" && define.amd) { define([], f) } else { var g; if (typeof window !== "undefined") { g = window } else if (typeof global !== "undefined") { g = global } else if (typeof self !== "undefined") { g = self } else { g = this } g.WavPlayer = f() } })(function () {
    var define, module, exports; return (function e(t, n, r) { function s(o, u) { if (!n[o]) { if (!t[o]) { var a = typeof require == "function" && require; if (!u && a) return a(o, !0); if (i) return i(o, !0); var f = new Error("Cannot find module '" + o + "'"); throw f.code = "MODULE_NOT_FOUND", f } var l = n[o] = { exports: {} }; t[o][0].call(l.exports, function (e) { var n = t[o][1][e]; return s(n ? n : e) }, l, l.exports, e, t, n, r) } return n[o].exports } var i = typeof require == "function" && require; for (var o = 0; o < r.length; o++)s(r[o]); return s })({
        1: [function (require, module, exports) {
            "use strict";

            Object.defineProperty(exports, "__esModule", {
                value: true
            });

            var _typeof = typeof Symbol === "function" && typeof Symbol.iterator === "symbol" ? function (obj) { return typeof obj; } : function (obj) { return obj && typeof Symbol === "function" && obj.constructor === Symbol ? "symbol" : typeof obj; };

            var _wavify = require("./wavify");

            var _wavify2 = _interopRequireDefault(_wavify);

            var _concat = require("./concat");

            var _concat2 = _interopRequireDefault(_concat);

            function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

            var pad = function pad(buffer) {
                var currentSample = new Float32Array(1);

                buffer.copyFromChannel(currentSample, 0, 0);

                var wasPositive = currentSample[0] > 0;

                for (var i = 0; i < buffer.length; i += 1) {
                    buffer.copyFromChannel(currentSample, 0, i);

                    if (wasPositive && currentSample[0] < 0 || !wasPositive && currentSample[0] > 0) {
                        break;
                    }

                    currentSample[0] = 0;
                    buffer.copyToChannel(currentSample, 0, i);
                }

                buffer.copyFromChannel(currentSample, 0, buffer.length - 1);

                wasPositive = currentSample[0] > 0;

                for (var _i = buffer.length - 1; _i > 0; _i -= 1) {
                    buffer.copyFromChannel(currentSample, 0, _i);

                    if (wasPositive && currentSample[0] < 0 || !wasPositive && currentSample[0] > 0) {
                        break;
                    }

                    currentSample[0] = 0;
                    buffer.copyToChannel(currentSample, 0, _i);
                }

                return buffer;
            };

            var WavPlayer = function WavPlayer() {
                var context = void 0;

                var hasCanceled_ = false;

                var _play = function _play(url) {
                    var nextTime = 0;

                    var audioStack = [];

                    hasCanceled_ = false;

                    context = new AudioContext();

                    var scheduleBuffersTimeoutId = null;

                    var scheduleBuffers = function scheduleBuffers() {
                        if (hasCanceled_) {
                            scheduleBuffersTimeoutId = null;

                            return;
                        }

                        while (audioStack.length > 0 && audioStack[0].buffer !== undefined && nextTime < context.currentTime + 2) {
                            var currentTime = context.currentTime;

                            var source = context.createBufferSource();

                            var segment = audioStack.shift();

                            source.buffer = pad(segment.buffer);
                            source.connect(context.destination);

                            if (nextTime == 0) {
                                nextTime = currentTime + 0.2; /// add 700ms latency to work well across systems - tune this if you like
                            }

                            var duration = source.buffer.duration;
                            var offset = 0;

                            if (currentTime > nextTime) {
                                offset = currentTime - nextTime;
                                nextTime = currentTime;
                                duration = duration - offset;
                            }

                            source.start(nextTime, offset);
                            source.stop(nextTime + duration);

                            nextTime += duration; // Make the next buffer wait the length of the last buffer before being played
                        }

                        scheduleBuffersTimeoutId = setTimeout(function () {
                            return scheduleBuffers();
                        }, 500);
                    };

                    return fetch(url).then(function (response) {
                        var reader = response.body.getReader();

                        // This variable holds a possibly dangling byte.
                        var rest = null;

                        var isFirstBuffer = true;
                        var numberOfChannels = void 0,
                            sampleRate = void 0;

                        var read = function read() {
                            return reader.read().then(function (_ref) {
                                var value = _ref.value;
                                var done = _ref.done;

                                if (hasCanceled_) {
                                    reader.cancel();

                                    return;
                                }
                                if (value && value.buffer) {
                                    var _ret = function () {
                                        var buffer = void 0,
                                            segment = void 0;

                                        if (rest !== null) {
                                            buffer = (0, _concat2.default)(rest, value.buffer);
                                        } else {
                                            buffer = value.buffer;
                                        }

                                        // Make sure that the first buffer is lager then 44 bytes.
                                        if (isFirstBuffer && buffer.byteLength <= 44) {
                                            rest = buffer;

                                            read();

                                            return {
                                                v: void 0
                                            };
                                        }

                                        // If the header has arrived try to derive the numberOfChannels and the
                                        // sampleRate of the incoming file.
                                        if (isFirstBuffer) {
                                            isFirstBuffer = false;

                                            var dataView = new DataView(buffer);

                                            numberOfChannels = dataView.getUint16(22, true);
                                            sampleRate = dataView.getUint32(24, true);

                                            buffer = buffer.slice(44);
                                        }

                                        if (buffer.byteLength % 2 !== 0) {
                                            rest = buffer.slice(-2, -1);
                                            buffer = buffer.slice(0, -1);
                                        } else {
                                            rest = null;
                                        }

                                        segment = {};

                                        audioStack.push(segment);

                                        context.decodeAudioData((0, _wavify2.default)(buffer, numberOfChannels, sampleRate)).then(function (audioBuffer) {
                                            segment.buffer = audioBuffer;

                                            if (scheduleBuffersTimeoutId === null) {
                                                scheduleBuffers();
                                            }
                                        });
                                    }();

                                    if ((typeof _ret === "undefined" ? "undefined" : _typeof(_ret)) === "object") return _ret.v;
                                }

                                if (done) {
                                    return;
                                }

                                // continue reading
                                read();
                            });
                        };

                        // start reading
                        read();
                    });
                };

                return {
                    play: function play(url) {
                        return _play(url);
                    },
                    stop: function stop() {
                        hasCanceled_ = true;
                        if (context) {
                            context.close();
                        }
                    }
                };
            };

            exports.default = WavPlayer;

        }, { "./concat": 2, "./wavify": 4 }], 2: [function (require, module, exports) {
            "use strict";

            Object.defineProperty(exports, "__esModule", {
                value: true
            });
            // Concat two ArrayBuffers
            var concat = function concat(buffer1, buffer2) {
                var tmp = new Uint8Array(buffer1.byteLength + buffer2.byteLength);

                tmp.set(new Uint8Array(buffer1), 0);
                tmp.set(new Uint8Array(buffer2), buffer1.byteLength);

                return tmp.buffer;
            };

            exports.default = concat;

        }, {}], 3: [function (require, module, exports) {
            "use strict";

            Object.defineProperty(exports, "__esModule", {
                value: true
            });

            var _WavPlayer = require("./WavPlayer");

            var _WavPlayer2 = _interopRequireDefault(_WavPlayer);

            function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

            exports.default = _WavPlayer2.default;

            module.exports = _WavPlayer2.default;

        }, { "./WavPlayer": 1 }], 4: [function (require, module, exports) {
            "use strict";

            Object.defineProperty(exports, "__esModule", {
                value: true
            });

            var _concat = require("./concat");

            var _concat2 = _interopRequireDefault(_concat);

            function _interopRequireDefault(obj) { return obj && obj.__esModule ? obj : { default: obj }; }

            // Write a proper WAVE header for the given buffer.
            var wavify = function wavify(data, numberOfChannels, sampleRate) {
                var header = new ArrayBuffer(44);

                var d = new DataView(header);

                d.setUint8(0, "R".charCodeAt(0));
                d.setUint8(1, "I".charCodeAt(0));
                d.setUint8(2, "F".charCodeAt(0));
                d.setUint8(3, "F".charCodeAt(0));

                d.setUint32(4, data.byteLength / 2 + 44, true);

                d.setUint8(8, "W".charCodeAt(0));
                d.setUint8(9, "A".charCodeAt(0));
                d.setUint8(10, "V".charCodeAt(0));
                d.setUint8(11, "E".charCodeAt(0));
                d.setUint8(12, "f".charCodeAt(0));
                d.setUint8(13, "m".charCodeAt(0));
                d.setUint8(14, "t".charCodeAt(0));
                d.setUint8(15, " ".charCodeAt(0));

                d.setUint32(16, 16, true);
                d.setUint16(20, 1, true);
                d.setUint16(22, numberOfChannels, true);
                d.setUint32(24, sampleRate, true);
                d.setUint32(28, sampleRate * 1 * 2);
                d.setUint16(32, numberOfChannels * 2);
                d.setUint16(34, 16, true);

                d.setUint8(36, "d".charCodeAt(0));
                d.setUint8(37, "a".charCodeAt(0));
                d.setUint8(38, "t".charCodeAt(0));
                d.setUint8(39, "a".charCodeAt(0));
                d.setUint32(40, data.byteLength, true);

                return (0, _concat2.default)(header, data);
            };

            exports.default = wavify;

        }, { "./concat": 2 }]
    }, {}, [3])(3)
});