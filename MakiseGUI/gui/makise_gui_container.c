#include "makise_gui_container.h"

void makise_g_cont_add(MContainer * cont, MElement *el)
{
    if(cont == 0 || el == 0)
	return;
    el->parent = cont;
    el->gui = cont->gui;
    if(cont->first == 0) //empty conainer
    {
	cont->first = el;
	cont->last = el;
	cont->focused = 0;
	el->next = 0;
	el->prev = 0;
	
	return;
    }
    MElement *m = cont->last;
//    if(m == 0) //if last accidently didn't setted.
//    {
//	m = cont->first;
//	while (m->next != 0)
//	    m = m->next;
//    }
    m->next = el;
    el->prev = m;
    el->next = 0;
    cont->last = el;
}
void makise_g_cont_rem(MElement *el)
{
    if(el == 0 || el->parent == 0)
	return;
    MContainer *c = el->parent;

    if(el->prev != 0 && el->next != 0) //if element is not last or first
    {
	el->parent = 0;
	el->prev->next = el->next;
	el->next->prev = el->prev;
	el->next = el->prev = 0;
    }
    else if (el->prev == 0 && el->next != 0) //first element
    {
	el->parent = 0;
	c->first = el->next;
	el->next->prev = 0;
	el->next = 0;
    }
    else if (el->prev != 0 && el->next == 0) //last element
    {
	el->parent = 0;
	el->prev->next = 0;
	c->last = el->prev;
	el->prev = 0;
    }
    else //only member of container
    {
	el->parent = 0;
	c->last = c->first = 0;
	el->next = el->prev = 0;
    }
}
int32_t makise_g_cont_insert(MContainer * cont, MElement *el, uint32_t index)
{
    if(cont == 0 || el == 0)
	return -1;
    
    makise_g_cont_rem(el); //remove element from previous parent

    uint32_t i = 0;
    MElement *e = cont->first;

    if(index == 0 || cont->first == 0) //required index is 0 or if there is no elements in container
    {
	el->gui = cont->gui;
	el->prev = 0;
	if(cont->first == 0)
	{
	    cont->last = el;
	}
	el->next = cont->first;
	cont->first = el;
	return 0;
    }
    
    while(e != 0)
    {
	i++;
	if(i == index) //if it is requested position
	{
	    el->gui = cont->gui;
	    el->next = e->next;
	    el->prev = e;
	    //e->prev = el;
	    if(e->next == 0) //if element was last
	    {
		cont->last = el;
	    }
	    else
	    {
		e->next->prev = el;
		//e->next = el;
	    }
	    e->next = el;
	    return i;
	}
	if(e->next == 0) //if element is last
	{
	    el->gui = cont->gui;
	    cont->last = el;
	    e->next = el;
	    el->next = 0;
	    el->prev = e;
	    return i;
	}
	e = e->next;
    }
    return -1;
}
int32_t makise_g_cont_contains(MContainer * cont, MElement *el)
{
    if(cont == 0 || el == 0)
	return -1;
    if(el->parent == 0 || //if element has no parent
       el->parent != cont) //if element's parent isn't requested container
	return -1;
    
    return makise_g_cont_index(el);
}
int32_t makise_g_cont_index(MElement *el)
{
    if(el == 0 || el->parent == 0 || el->parent->first == 0)
	return - 1;
    
    uint32_t i = 0;
    MElement *e = el->parent->first;

    while (e != 0) {
	if(e == el)
	    return i;
	e = e->next;
	i++;
    }
    return -1;
}


uint8_t makise_g_cont_call_common_predraw(MElement *b)
{
    if(b != 0 && b->parent != 0 && b->parent->position != 0)
    {
	b->position.real_x = b->position.x + b->parent->position->real_x;
	b->position.real_y = b->position.y + b->parent->position->real_y;
    }
    else
    {
	b->position.real_x = b->position.x;
	b->position.real_y = b->position.y;
    }
    return M_OK;
}
uint8_t makise_g_cont_call   (MContainer *cont, uint8_t type)
{
    if(cont == 0)
	return M_ZERO_POINTER;
    if(cont->first == 0)
	return M_ZERO_POINTER;

    if(cont->el != 0) //container itself
    {
	if(type == M_G_CALL_PREDRAW)
	    makise_g_cont_call_common_predraw(cont->el);
//	m_element_call(cont->el, type);
    }
    
    MElement *e = cont->first;

    while(e != 0)
    {
	if(type == M_G_CALL_PREDRAW)
	    makise_g_cont_call_common_predraw(e);
	m_element_call(e, type);
	e = e->next;
    }
    return M_OK;
}

MInputResultEnum makise_g_cont_input  (MContainer *cont, MInputData data)
{
    if(cont == 0)
	return M_ZERO_POINTER;

#if MAKISE_GUI_INPUT_POINTER_ENABLE == 1
    
#endif
    
    if(cont->focused != 0)
	return cont->focused->input(cont->focused, data);

    return M_INPUT_NOT_HANDLED;
}

MFocusEnum makise_g_cont_focus_next(MContainer *cont)
{
    if(cont == 0)
	return M_ZERO_POINTER;

    if(cont->focused != 0)
    {
	if(cont->focused->focus != 0 && cont->focused->focus(cont->focused, M_G_FOCUS_NEXT) != M_G_FOCUS_OK)
	{
	    //if focused element didn't switched focus by itself or focus function pointer equals to zero, then let's try to do it by ourself.
	    MElement *e = cont->focused->next;
	    uint8_t res;
	    while(e != 0)
	    {
		if(e->focus_prior != 0)
		{
		    if((res = makise_g_focus(e, M_G_FOCUS_GET_NEXT)) == M_G_FOCUS_OK)
			return M_G_FOCUS_OK;
		    //printf("res id %d %d\n", e->id, res);
		}
		
		e = e->next;
	    }

	    //if isn't no more elements requiring focus
	    if(cont->el == 0) //if cont is host container
	    {
		e = cont->first;
		while(e != 0)
		{
		    if(e->focus_prior != 0)
		    {
			if(makise_g_focus(e, M_G_FOCUS_GET_NEXT) == M_G_FOCUS_OK)
			    return M_G_FOCUS_OK;
		    }
		    e = e->next;
		}
	    }
	    return M_G_FOCUS_NOT_NEEDED;
	}
	else
	    return M_G_FOCUS_OK;
    }
    else
    {
	MElement *e = cont->first;
	while(e != 0)
	{
	    if(e->focus_prior != 0)
	    {
		if(makise_g_focus(e, M_G_FOCUS_GET_NEXT) == M_G_FOCUS_OK)
		{
//		    makise_g_focus(e, M_G_FOCUS_NEXT);
		    return M_G_FOCUS_OK;
		}
	    }
	    e = e->next;
	}
	return M_G_FOCUS_NOT_NEEDED;
    }
}
    
MFocusEnum makise_g_cont_focus_prev(MContainer *cont)
{
    if(cont == 0)
	return M_ZERO_POINTER;

    if(cont->focused != 0)
    {
	if(cont->focused->focus != 0 && cont->focused->focus(cont->focused, M_G_FOCUS_PREV) != M_G_FOCUS_OK)
	{
	    //if focused element didn't switched focus by itself or focus function pointer equals to zero, then let's try to do it by ourself.
	    MElement *e = cont->focused->prev;
	    while(e != 0)
	    {
		if(e->focus_prior != 0)
		{
		    if(makise_g_focus(e, M_G_FOCUS_GET_PREV) == M_G_FOCUS_OK)
		    {
			return M_G_FOCUS_OK;
		    }
		}
		e = e->prev;
	    }

	    //if isn't no more elements requiring focus
	    if(cont->el == 0) //if cont is host container
	    {
		e = cont->last;
		while(e != 0)
		{
		    if(e->focus_prior != 0)
		    {
			if(makise_g_focus(e, M_G_FOCUS_GET_PREV) == M_G_FOCUS_OK)
			    return M_G_FOCUS_OK;
		    }
		    e = e->prev;
		}
	    }
	    return M_G_FOCUS_NOT_NEEDED;
	}
	else
	    return M_G_FOCUS_OK;
    }
    else
    {
	MElement *e = cont->last;
	while(e != 0)
	{
	    if(e->focus_prior != 0)
	    {
		if(makise_g_focus(e, M_G_FOCUS_GET_PREV) == M_G_FOCUS_OK)
		{
//		    makise_g_focus(e, M_G_FOCUS_PREV);
		    return M_G_FOCUS_OK;
		}
	    }
	    e = e->prev;
	}
	return M_G_FOCUS_NOT_NEEDED;
    }
}

MElement* makise_g_cont_element_on_point(MContainer *cont, int32_t  x, int32_t y)
{
    if(cont == 0 || cont->first == 0)
	return 0;

    MElement *e = cont->last; //last - means upper in draw queue

//    printf("point %d %d %d\n", x, y, e);
    
    while(e != 0)
    {
//	printf("check %d\n", e->id);
	if(x >= e->position.real_x && x < e->position.real_x + e->position.width &&
	   y >= e->position.real_y && y < e->position.real_y + e->position.height)
	{
//	    printf("ok %d\n", e->id);
	    //if point is in element's area
	    if(e->is_parent == 1)
	    {
		//if parent
		if(e->children != 0)
		{
		    return makise_g_cont_element_on_point(e->children, x, y);
		}
	    }
	    return e; //element found
	}
	e = e->prev;
    }
    return 0;
}
