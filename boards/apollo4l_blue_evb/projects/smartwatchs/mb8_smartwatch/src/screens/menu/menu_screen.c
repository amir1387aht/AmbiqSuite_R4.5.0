/**
 * @file menu_screen.c
 * @brief Implementation of vertical scrollable menu with icons, labels, and button actions
 *        Optimized for 192x490 display
 */

/*********************
 *      INCLUDES
 *********************/
#include "./menu_screen.h"

/*********************
 *      DEFINES
 *********************/
#define MENU_ITEM_COUNT 18
#define MENU_ICON_SIZE  100
#define MENU_PADDING    0
#define MENU_ITEM_SPACE 0
#define TEXT_ICON_SPACE 75
#define BOTTOM_PADDING  50

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    const char* symbol;
    const char* text;
    void (*action)(void);
} menu_item_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void create_menu_item(lv_obj_t * parent, const char* symbol, const char* text, void (*action)(void), int index);
static void menu_button_event_handler(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
static const menu_item_t menu_items[MENU_ITEM_COUNT] = {
    {LV_SYMBOL_HOME, "Home", home_action},
    {LV_SYMBOL_SETTINGS, "Settings", NULL},
    {LV_SYMBOL_CALL, "Call", NULL},
    {LV_SYMBOL_WIFI, "WiFi", NULL},
    {LV_SYMBOL_BLUETOOTH, "BT", NULL},
    {LV_SYMBOL_BATTERY_FULL, "Battery", NULL},
    {LV_SYMBOL_AUDIO, "Audio", NULL},
    {LV_SYMBOL_BELL, "Alerts", NULL},
    {LV_SYMBOL_LIST, "List", NULL},
    {LV_SYMBOL_CLOSE, "Close", NULL},
    {LV_SYMBOL_DOWNLOAD, "Download", NULL},
    {LV_SYMBOL_UPLOAD, "Upload", NULL},
    {LV_SYMBOL_PLAY, "Play", NULL},
    {LV_SYMBOL_PAUSE, "Pause", NULL},
    {LV_SYMBOL_REFRESH, "Refresh", NULL},
    {LV_SYMBOL_GPS, "GPS", NULL},
    {LV_SYMBOL_OK, "OK", NULL},
    {LV_SYMBOL_POWER, "Power", NULL}
};

lv_obj_t * screen_menu_create(void)
{
    // Set screen background
    lv_obj_set_style_bg_color(lv_scr_act(), lv_color_black(), LV_PART_MAIN);

    // Create main container
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
    lv_obj_set_size(cont, LV_PCT(100), LV_PCT(100)); 
    lv_obj_set_style_bg_color(cont, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(cont, 0, LV_PART_MAIN);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE); // Disable scrolling for main container
    
    // Create scrollable container
    lv_obj_t * scrollable = lv_obj_create(cont);
    lv_obj_set_size(scrollable, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_bg_color(scrollable, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(scrollable, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(scrollable, MENU_PADDING, LV_PART_MAIN);
    lv_obj_set_style_pad_bottom(scrollable, BOTTOM_PADDING, LV_PART_MAIN); // Add padding at the bottom
    
    // Configure flex layout for scrollable
    lv_obj_set_flex_flow(scrollable, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(scrollable, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Configure scrolling
    lv_obj_set_scroll_dir(scrollable, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(scrollable, LV_SCROLLBAR_MODE_OFF); // Hide scrollbar

    // Create menu items
    for(int i = 0; i < MENU_ITEM_COUNT; i++)
        create_menu_item(scrollable, menu_items[i].symbol, menu_items[i].text, menu_items[i].action, i);
    
    return cont;
}

static void create_menu_item(lv_obj_t * parent, const char* symbol, const char* text, void (*action)(void), int index)
{
    // Calculate item height - more compact design
    uint16_t item_height = MENU_ICON_SIZE + TEXT_ICON_SPACE;
    
    // Create item container
    lv_obj_t * item_cont = lv_obj_create(parent);
    lv_obj_set_size(item_cont, 180, item_height);
    lv_obj_set_style_bg_color(item_cont, lv_color_black(), LV_PART_MAIN);
    lv_obj_set_style_border_width(item_cont, 0, LV_PART_MAIN);
    lv_obj_clear_flag(item_cont, LV_OBJ_FLAG_SCROLLABLE);
    
    // Set spacing between items
    lv_obj_set_style_pad_bottom(item_cont, MENU_ITEM_SPACE, LV_PART_MAIN);
    
    // Create circle background for icon
    lv_obj_t * circle = lv_obj_create(item_cont);
    lv_obj_set_size(circle, MENU_ICON_SIZE, MENU_ICON_SIZE);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
    lv_obj_set_style_bg_color(circle, lv_color_make(40, 40, 40), LV_PART_MAIN);
    lv_obj_set_style_border_width(circle, 0, LV_PART_MAIN);
    lv_obj_clear_flag(circle, LV_OBJ_FLAG_SCROLLABLE);  // Disable scrolling for circle
    lv_obj_align(circle, LV_ALIGN_TOP_MID, 0, 0); // Position at top-center
    
    // Create icon inside circle
    lv_obj_t * icon = lv_label_create(circle);
    lv_label_set_text(icon, symbol);
    lv_obj_set_style_text_font(icon, &lv_font_montserrat_48, LV_PART_MAIN);
    lv_obj_set_style_text_color(icon, lv_color_white(), LV_PART_MAIN);
    lv_obj_clear_flag(icon, LV_OBJ_FLAG_SCROLLABLE);  // Disable scrolling for icon
    lv_obj_center(icon); // Center the icon in the circle
    
    // Create text label
    lv_obj_t * label = lv_label_create(item_cont);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_28, LV_PART_MAIN);
    lv_obj_set_style_text_color(label, lv_color_white(), LV_PART_MAIN);
    lv_obj_clear_flag(label, LV_OBJ_FLAG_SCROLLABLE);  // Disable scrolling for label
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0); // Align to bottom-center
    
    // Make everything clickable
    lv_obj_add_flag(item_cont, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(circle, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(icon, LV_OBJ_FLAG_CLICKABLE);
    
    // Add event callbacks to ALL elements to ensure clicks work everywhere
    if (action) {
        lv_obj_add_event_cb(item_cont, menu_button_event_handler, LV_EVENT_CLICKED, (void*)action);
        lv_obj_add_event_cb(circle, menu_button_event_handler, LV_EVENT_CLICKED, (void*)action);
        lv_obj_add_event_cb(icon, menu_button_event_handler, LV_EVENT_CLICKED, (void*)action);
        lv_obj_add_event_cb(label, menu_button_event_handler, LV_EVENT_CLICKED, (void*)action);
    }
}

static void menu_button_event_handler(lv_event_t * e)
{
    void (*action)(void) = (void (*)())lv_event_get_user_data(e);
    if (action) action();
}

static void home_action(void) {
    lv_obj_t * popup = lv_msgbox_create(NULL, "Home", "Home button pressed!", NULL, true);
    lv_obj_center(popup);
    device_vibrate(2);
}