const int kRowSize = 500;
const int kColSize = 500;
const int kMaxNode = 1e3 + 10;
const int kBufferSize = 20;
const int kAlpha = 3;
const int kSnapshotPerRound = 1000;
const int kThreadNum = 16;
const int kParalleSize = kRowSize * kColSize / (kThreadNum*8) + 10;
