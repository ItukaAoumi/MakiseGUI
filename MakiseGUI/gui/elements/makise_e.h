#ifndef MAKISE_E_H
#define MAKISE_E_H      1

#ifdef __cplusplus
extern "C" {
#endif
#include "makise_config.h"
    
#include "makise_gui_elements.h"
#include "makise_gui_container.h"

#define M_E_MUTEX_REQUEST(e) m_element_mutex_request(&(e)->el);
#define M_E_MUTEX_RELEASE(e) m_element_mutex_release(&(e)->el);    
    
#include "fonts.h"
#include "makise_e_buttons.h"
#include "makise_e_canvas.h"
#include "makise_e_lable.h"
#include "makise_e_progress_bar.h"
#include "makise_e_slider.h"
#include "makise_e_slist.h"
#include "makise_e_tabs.h"
#include "makise_e_text_field.h"
#include "makise_e_toggle.h"
#include "makise_e_message_window.h"
#include "makise_e_fsviewer.h"
#include "makise_e_play_list.h"

void _m_e_helper_draw_box_param ( MakiseBuffer* b, MPosition *p, uint32_t border_color, uint32_t bg_color, uint16_t double_border );
void _m_e_helper_draw_box(MakiseBuffer* b, MPosition *p, MakiseStyleTheme *th );

#ifdef __cplusplus
}
#endif

#endif
