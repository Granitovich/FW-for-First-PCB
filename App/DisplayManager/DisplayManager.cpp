// // Початкова ініціалізація екрану / заставки (створення init image)
//     void createInitImage() {
//         driver.fillScreen(BLACK);
//         // Малюємо логотип або текст заставки
//         driver.drawString(10, 30, "MemoryHub FW", WHITE);
//         driver.drawString(10, 50, "Initializing...", GRAY);
//         currentState = DisplayState::SPLASH_SCREEN;
//     }

//     // Головна функція створення та відрисовки меню
//     void createMenu(const char* title, const char* items[], uint8_t itemCount) {
//         driver.fillScreen(BLACK);
        
//         // Виводимо заголовок меню
//         driver.drawString(10, 5, title, YELLOW);
//         driver.drawLine(0, 18, 128, 18, GRAY); // Розділювальна лінія

//         // Виводимо пункти меню
//         int y = 25;
//         for (uint8_t i = 0; i < itemCount; i++) {
//             if (i == menuIndex) {
//                 // Виділяємо активний пункт прямокутником або інверсією
//                 driver.fillRect(0, y - 2, 128, 16, DARK_GRAY);
//                 driver.drawString(10, y, items[i], WHITE);
//             } else {
//                 driver.drawString(10, y, items[i], LIGHT_GRAY);
//             }
//             y += 18;
//         }
//         currentState = DisplayState::MAIN_MENU;
//     }

//     // Керування курсором (наприклад, при натисканні кнопок або енкодера)
//     void moveCursorUp(uint8_t maxItems) {
//         if (menuIndex > 0) {
//             menuIndex--;
//         } else {
//             menuIndex = maxItems - 1; // Зациклювання вгору
//         }
//     }

//     void moveCursorDown(uint8_t maxItems) {
//         if (menuIndex < maxItems - 1) {
//             menuIndex++;
//         } else {
//             menuIndex = 0; // Зациклювання вниз
//         }
//     }

//     uint8_t getSelectedIndex() const {
//         return menuIndex;
//     }

//     DisplayState getState() const {
//         return currentState;
//     }