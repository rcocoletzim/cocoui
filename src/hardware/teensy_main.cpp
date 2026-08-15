#include <Arduino.h>
#include <SPI.h>

#include "cocoui/engine.hpp" 

// ============================================================================
// 1. PHYSICAL HARDWARE DRIVER
// ============================================================================
class TeensyRA8875Driver : public cocoui::hal::DisplayDriver<uint16_t> {
   private:
    uint8_t cs_pin_;

    void set_window(int16_t x, int16_t y, int16_t w, int16_t h) {
        // Hardware specific commands to set the active drawing window
        // e.g., ra8875_ptr->setWindow(x, y, w, h);
    }

   public:
    explicit TeensyRA8875Driver(uint8_t cs) : cs_pin_(cs) {}

    void flush_area(const cocoui::Rect& area, const uint16_t* color_data) override {
        uint32_t byte_count = area.size.width * area.size.height * sizeof(uint16_t);

        // Cortex-M7 Data Cache Maintenance (CRITICAL for DMA/SPI)
        arm_dcache_flush((void*)color_data, byte_count);

        set_window(area.left(), area.top(), area.size.width, area.size.height);

        // SPI Block Transfer
        SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0)); 
        digitalWrite(cs_pin_, LOW);

        SPI.transfer(0x80); // CMD_WRITE
        SPI.transfer(0x02); // MRWC (Memory Read/Write Command)
        SPI.transfer(0x00); // DATA_WRITE

        SPI.transfer((void*)color_data, nullptr, byte_count);

        digitalWrite(cs_pin_, HIGH);
        SPI.endTransaction();
    }
};

// ============================================================================
// 2. APPLICATION LOGIC (State & View)
// ============================================================================
struct AppState {
    cocoui::Color button_color = cocoui::Colors::Red;
};

// Purely declarative UI definition. No logic, just structure.
constexpr auto build_ui(AppState& state) {
    // Return your UI tree here using cocoui::make_canvas, make_button, etc.
    return cocoui::make_canvas(/* ... */); 
}

// ============================================================================
// 3. GLOBAL STATIC INSTANTIATION
// ============================================================================
// Allocate Framebuffer in the BSS section (Zero Heap Allocation)
cocoui::Framebuffer<800, 480, cocoui::PixelFormat::RGB565> framebuffer;
TeensyRA8875Driver display_driver(10);

// Initialize the UI Engine
auto ui_engine = cocoui::make_engine(AppState{}, build_ui, framebuffer, display_driver, cocoui::Color(30, 30, 30));


// ============================================================================
// 4. MAIN LIFECYCLE (Bare-metal or RTOS compliant)
// ============================================================================
void setup() {
    Serial.begin(115200);
    
    // 1. Initialize Base Hardware
    pinMode(10, OUTPUT);
    digitalWrite(10, HIGH);
    SPI.begin();
    
    // Initialize Display Controller (e.g., wake up RA8875, configure PLL)
    // RA8875_Init(); 

    // 2. Boot the UI Engine
    ui_engine.start();
}

void loop() {
    // 1. Poll Physical Hardware (Non-blocking)
    // In a real scenario, this would read from an I2C Touch Controller (e.g. FT5206)
    bool is_touched = false; // ra8875.touched();
    int touch_x = 0;         // ra8875.touchReadX();
    int touch_y = 0;         // ra8875.touchReadY();

    // 2. Inject Events into the Engine
    ui_engine.inject_touch(is_touched, touch_x, touch_y);

    // 3. Tick the Engine (0% CPU usage if state hasn't changed)
    ui_engine.tick();

    // 4. Execute Mission-Critical Tasks
    // Process_CAN_Bus();
    // Update_PID_Controllers();
}