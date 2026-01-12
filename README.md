# VectorDebug USB Data Protocol and Debug Method

**VectorDebug** is a software tool designed for **debugging ESCs and motor controllers** operating in **sensorless or field-oriented control (FOC) mode**, where **high-frequency data acquisition** from the controller is required (up to **10–20 kHz**). It allows real-time capture and transmission of debug variables and CLI messages over **SPI → Blackpill → USB**.

---

## 1. Hardware Setup

The hardware connection is organized as follows:

- **ESC** (Electronic Speed Controller):
  - SPI master for debug data
  - Optional CLI interface via UART
- **Blackpill Board**:
  - SPI slave receiving debug frames
  - USB CDC device exposing collected data to PC
  - Optional UART passthrough for CLI commands

**Pinout Example:**

| Signal      | ESC          | Blackpill |
|------------ |------------- |-----------|
| SPI_MOSI    | Master MOSI  | Slave MOSI|
| SPI_MISO    | Master MISO  | Slave MISO|
| SPI_SCK     | Master SCK   | Slave SCK |
| SPI_NSS     | Master NSS   | Slave NSS |
| UART_TX/RX  | Optional CLI | Blackpill |

> Note: SPI runs at up to 4.5 Mbps to allow high-frequency debug frame transmission.

---

## 2. SPI Debug Frame

The ESC sends fixed-size debug frames over SPI. Each frame is represented as:

```c
typedef struct {
    uint16_t seq;          // Sequential frame number
    int16_t  payload[6];   // 6 debug variables
} McDbgPayload;
````

* `seq` increments with each frame, allowing detection of **lost frames**.
* `payload` contains **arbitrary variables**, e.g., currents, voltages, or control signals.

Frames are collected on the Blackpill into a **buffer** for batching before USB transmission.

---

## 3. Buffering Architecture on Blackpill

### 3.1 Double Buffer (Ping-Pong)

```
SPI DMA (slave) IRQ → fills active buffer
[ Buffer A ]  ← active
[ Buffer B ]  ← ready for USB

When active buffer is full or timer triggers:
    swap buffers
    USB reads ready buffer
```

* SPI writes continuously to **active buffer** at high frequency.
* USB reads **ready buffer** in batches (1–10 ms typical), ensuring **non-blocking high-frequency capture**.
* This avoids dropped frames and allows batch transmission over USB.

---

## 4. USB Packet Structure (`McDbgPacket_t`)

Multiple `McDbgPayload` frames are packed into a single USB packet:

| Field        | Description                                                         |
| ------------ | ------------------------------------------------------------------- |
| Message Type | `TYPE_DBG_FRAME1 = 3` (debug frames), `TYPE_CLI = 2` (CLI messages) |
| Payload      | Multiple `McDbgPayload` messages (2–4 KB typical)                   |
| CRC16        | CRC16-CCITT (initial value = 0xFFFF)                                |

---

### 4.1 Packing Messages

1. Collect `McDbgPayload` messages from SPI buffer.
2. Call:

```c
uint16_t packet_len = mcdbgPackMsg(TYPE_DBG_FRAME1, payloads, payload_count * sizeof(McDbgPayload), packet_buf);
```

3. Compute **CRC16-CCITT** over the packet.

---

### 4.2 COBS Encoding for USB

1. Encode packet using **COBS** (`mcdbgEncode()`).
2. Append a **trailing zero byte** to mark the end of the USB frame:

```c
uint16_t cobs_len = mcdbgEncode(packet_buf, packet_len, cobs_buf);
cobs_buf[cobs_len++] = 0; // zero terminator
```

3. Send `cobs_buf` over **USB CDC**.

---

### 4.3 Receiving on PC

1. Read bytes until **0 terminator**.
2. Apply **COBS decode** to reconstruct the packet.
3. Validate **CRC16**.
4. Parse `McDbgPacket_t` to extract:

   * Debug frames (`McDbgPayload`)
   * CLI messages if present

---

## 5. Manual Data Packing (Without Blackpill Firmware)

If you are **not using Blackpill firmware**, follow these steps:

```c
McDbgPayload payloads[N];

for (int i = 0; i < N; i++) {
    payloads[i].seq = current_seq++;
    payloads[i].payload[0] = var1;
    payloads[i].payload[1] = var2;
    ...
    payloads[i].payload[5] = var6;
}

/* Pack frames into McDbgPacket */
uint8_t packet_buf[MAX_USB_SIZE];
uint16_t packet_len = mcdbgPackMsg(TYPE_DBG_FRAME1, payloads, sizeof(payloads), packet_buf);

/* COBS encode and append zero terminator */
uint8_t cobs_buf[MAX_USB_SIZE];
uint16_t cobs_len = mcdbgEncode(packet_buf, packet_len, cobs_buf);
cobs_buf[cobs_len++] = 0;

/* Send via USB CDC */
usb_send(cobs_buf, cobs_len);
```

---

## 6. Flow Diagram

```text
ESC (SPI master)
   │
   │  High-frequency debug frames (4.5 Mbps)
   ▼
Blackpill (SPI slave, DMA)
   ├─ Ping-Pong buffer
   │     ┌───────────────┐
   │     │ Active buffer │← SPI DMA fills
   │     └───────────────┘
   │     ┌───────────────┐
   │     │ Ready buffer  │→ USB CDC
   │     └───────────────┘
   ▼
USB CDC → PC
   ├─ Read until zero
   ├─ COBS decode
   ├─ CRC16 check
   └─ Extract debug frames / CLI messages
```

---

## 7. Notes

* Sequence numbers (`seq`) allow **detection of lost or dropped frames**.
* Multiple frames are batched to **minimize USB overhead**.
* CLI messages (`TYPE_CLI`) can be interleaved with debug frames.
* Maximum packet size: typically **2–4 KB**.
* This architecture ensures **deterministic, high-frequency capture** even under heavy load.

---
## 8. Summary

1. ESC transmits **high-frequency debug frames** over SPI.
2. Blackpill **buffers frames**, packs them into `McDbgPacket_t`, computes CRC16.
3. Packet is **COBS-encoded**, terminated with zero, and sent over USB.
4. PC decodes, validates CRC, and extracts **payload and CLI messages**.
5. Supports **bidirectional CLI communication** and **high-frequency telemetry**.

This method provides a **robust, real-time debugging protocol** for ESCs and FOC motor controllers.
```
