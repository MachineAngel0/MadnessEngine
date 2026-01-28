#ifndef UI_H
#define UI_H


#include "math_types.h"
#include "text.h"


//IMMEDIATE MODE UI

//TODO: this should just get replaced
//we can just keep setting whatever the size of the window is at the start/end of each frame,
//it literally won't be a noticeable performance cost


typedef enum Alignment
{
    CENTER,
    LEFT,
    RIGHT,
    //JUSTIFIED,
} Alignment;


typedef struct UIID
{
    int ID;
    int layer;
} UIID;

//TODO: update to SOA
typedef struct UI_BUTTON
{
    UIID id;
    vec2 position;
    vec2 screen_percentage;
    vec3 color;
    vec3 hover_color;
    vec3 pressed_color;
} UI_BUTTON;

typedef struct UI_DRAW_INFO
{
    vec3* vertices;
    u16* indices;

    u32 vertex_count;
    u32 index_count;

    UI_BUTTON* UI_Objects; // darray or maybe even an allocator
} UI_DRAW_INFO;


typedef enum UI_Type
{
    color,
    texture,
} UI_Type;


typedef struct UI_STATE
{

    vec2 screen_size;

    UIID hot;
    UIID active;

    int id_generation_number = -1;

    int mouse_down = 0;
    int mouse_released = 0;
    vec2 mouse_pos = {-1, -1};

    //TODO: on UI update, we can just pass the screen size
    vec2 screen_size{800.0f, 600.0f};

    UI_DRAW_INFO draw_info{};
    Text_System text_system;

} UI_System;

UI_System* init_ui_state()
{
    UI_System* ui_state = (UI_System*)(malloc(sizeof(UI_System)));
    ui_state->active.ID = -1;
    ui_state->active.layer = -1;
    ui_state->hot.ID = -1;
    ui_state->hot.layer = -1;


    text_system_init(ui_state->text_system);

    return ui_state;
}



typedef struct TRANSFORM_2D
{
    glm::vec2 translation{};
    float rotation{};
    glm::vec2 scale{1.0f, 1.0f};

    glm::mat2 mat2()
    {
        float s = glm::sin(rotation);
        float c = glm::cos(rotation);
        glm::mat2 rotMatrix{{scale.x, s}, {c, scale.y}};

        glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
        return rotMatrix * scaleMat;
    }
} TRANSFORM_2D;


inline vec2 get_mouse_pos(GLFWwindow* window)
{
    double xpos;
    double ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    return glm::vec2(xpos, ypos);
}


inline std::vector<Vertex> UI_create_quad(glm::vec2 pos, glm::vec2 size, glm::vec3 color)
{
    return {
        {{pos.x, pos.y}, {color}},
        {{pos.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y}, {color}}
    };
}

inline std::vector<Vertex> UI_create_quad_screen_percentage(glm::vec2 pos, glm::vec2 size, glm::vec3 color)
{
    return {
        {{pos.x - size.x, pos.y - size.y}, {color}},
        {{pos.x - size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y + size.y}, {color}},
        {{pos.x + size.x, pos.y - size.y}, {color}}
    };
}


inline int ui_draw_rect(glm::vec2 pos, glm::vec2 size, glm::vec3 color, UI_System* ui_state)
{
    std::vector<Vertex> new_quad = UI_create_quad(pos, size, color);
    uint16_t base_index = static_cast<uint16_t>(ui_state->draw_info.vertex_info.dynamic_vertices.size());

    // Add vertices
    ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());

    // Add indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };

    ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());


    ui_state->draw_info.vertex_info.vertex_buffer_should_update = true;

    return ui_state->draw_info.vertex_info.mesh_id++;
};


inline int ui_draw_button_rect_screen_size_percentage(UI_System* ui_state, glm::vec2 pos, glm::vec2 screen_percentage,
                                                      glm::vec3 color = {1.0f, 1.0f, 1.0f},
                                                      glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                                                      pressed_color = {1.0f, 1.0f, 1.0f})
{
    //TODO: so rn its based on the topleft corner, but i want to expand the object from the center of where i put it not from top left

    //so in theory, giving a position on where it should be on the screen is fucking terrible, cause it will 100 go off the screen on a resize
    //so a -1,1, device coordinate wouldn't actually be a bad idea, we just specify where relatively on the screen we want it


    //tells us how much of the screen we want based on the size percentage
    if (screen_percentage.x > 100 || screen_percentage.x < 0 || screen_percentage.y > 100 || screen_percentage.y < 0)
    {
        printf("YOU ARE STUPID");
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("YOU ARE STUPID 2 ");
    }


    //so we specify what percent on the screen we want to start, and how big the elemenst are
    // 1920 * 1080    we want to position it at at 10% left, 10% down, then 10% wide and height

    //convert 0-100 -> 0-1
    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = screen_percentage / 100.0f;

    //these are the values in our vertex, but not in our ui_object
    /*
    glm::vec2 final_pos = {
        ui_state->draw_info.push_constants.screenSize.x * converted_pos.x,
        ui_state->draw_info.push_constants.screenSize.y * converted_pos.y
    };

    glm::vec2 final_size = {
        (ui_state->draw_info.push_constants.screenSize.x * converted_size.x) / 2,
        (ui_state->draw_info.push_constants.screenSize.y * converted_size.y) / 2
    };
    */

    glm::vec2 final_pos = {
        ui_state->push_constants.screenSize.x * converted_pos.x,
        ui_state->push_constants.screenSize.y * converted_pos.y
    };
    glm::vec2 final_size = {
        (ui_state->push_constants.screenSize.x * converted_size.x) / 2,
        (ui_state->push_constants.screenSize.y * converted_size.y) / 2
    };


    //std::vector<Vertex> new_quad = UI_create_quad(pos, screen_percentage, color);


    //std::vector<Vertex> new_quad = UI_create_quad(pos, screen_percentage, color);
    //this anchors the ui top left
    /*
    std::vector<Vertex> new_quad =  {
             {{final_pos.x , final_pos.y}, {color}},
                {{final_pos.x, final_pos.y + final_size.y}, {color}},
                {{final_pos.x + final_size.x, final_pos.y + final_size.y}, {color}},
                {{final_pos.x + final_size.x, final_pos.y}, {color}}
    };*/

    /*
    printf("Final Pos %f, %f\n", final_pos.x, final_pos.y);
    printf("Final SIZE %f, %f\n", final_size.x, final_size.y);
    */


    std::vector<Vertex> new_quad = UI_create_quad_screen_percentage(final_pos, final_size, color);

    /*
    std::cout << "QUAD:" << new_quad[0].pos.x << new_quad[0].pos.y << '\n';
    std::cout << "QUAD:" << new_quad[1].pos.x << new_quad[1].pos.y << '\n';
    std::cout << "QUAD:" << new_quad[2].pos.x << new_quad[2].pos.y << '\n';
    std::cout << "QUAD:" << new_quad[3].pos.x << new_quad[3].pos.y << '\n';
    */

    uint16_t base_index = static_cast<uint16_t>(ui_state->draw_info.vertex_info.dynamic_vertices.size());


    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };


    // Add vertices
    ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());

    // Add indices
    ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());


    ui_state->draw_info.vertex_info.vertex_buffer_should_update = true;

    UI_BUTTON new_ui_object = {
        {ui_state->draw_info.vertex_info.mesh_id, 0}, converted_pos, converted_size, color, hovered_color, pressed_color
    };

    //TODO: I MIGHT WANT TO REMOVE THIS the ID and have it passed in instead
    //increment id
    ui_state->draw_info.vertex_info.mesh_id++;

    ui_state->draw_info.UI_Objects.emplace_back(new_ui_object);

    return ui_state->draw_info.vertex_info.mesh_id;
};


inline void update_UI_on_resize(UI_System* ui_state, uint32_t screen_width, uint32_t screen_height)
{
    //update screen percentage
    //ui_state->push_constants.screenSize.x = swapchain_context.surface_capabilities.currentExtent.width;
    //ui_state->push_constants.screenSize.y = swapchain_context.surface_capabilities.currentExtent.height;
    ui_state->push_constants.screenSize.x = screen_width;
    ui_state->push_constants.screenSize.y = screen_height;


    //iterate over all ui objects and recalculate their size values

    //so in theory, giving a position on where it should be on the screen is fucking terrible, cause it will 100 go off the screen on a resize
    //so a -1,1, device coordinate wouldn't actually be a bad idea, we just specify where relatively on the screen we want it

    ui_state->draw_info.vertex_info.dynamic_vertices.clear();
    ui_state->draw_info.vertex_info.dynamic_indices.clear();

    for (auto& ui_object: ui_state->draw_info.UI_Objects)
    {
        //so we specify what percent on the screen we want to start, and how big the elemenst are
        // 1920 * 1080    we want to position it at at 10% left, 10% down, then 10% wide and height


        //these are the values in our vertex, but not in our ui_object
        glm::vec2 final_pos = {
            ui_state->push_constants.screenSize.x * ui_object.position.x,
            ui_state->push_constants.screenSize.y * ui_object.position.y
        };
        glm::vec2 final_size = {
            (ui_state->push_constants.screenSize.x * ui_object.screen_percentage.x) / 2,
            (ui_state->push_constants.screenSize.y * ui_object.screen_percentage.y) / 2
        };

        //glm::vec2 converted_pos = pos / 100.0f;
        //glm::vec2 converted_size = screen_percentage / 100.0f;

        //ui_object.position = final_pos;
        //ui_object.screen_percentage

        //std::vector<Vertex> new_quad = UI_create_quad(pos, screen_percentage, color);
        std::vector<Vertex> new_quad = UI_create_quad_screen_percentage(final_pos, final_size, ui_object.color);


        uint16_t base_index = static_cast<uint16_t>(ui_state->draw_info.vertex_info.dynamic_vertices.size());

        // Add vertices
        ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                                new_quad.begin(), new_quad.end());

        // Add indices (two triangles per quad)
        std::vector<uint16_t> quad_indices = {
            static_cast<uint16_t>(base_index + 0),
            static_cast<uint16_t>(base_index + 1),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 2),
            static_cast<uint16_t>(base_index + 3),
            static_cast<uint16_t>(base_index + 0)
        };

        ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                               quad_indices.begin(), quad_indices.end());
    }

    ui_state->draw_info.vertex_info.vertex_buffer_should_update = true;
}

inline bool is_ui_hot(UI_System* ui_state, int id)
{
    return ui_state->hot.ID && ui_state->hot.ID == id;
}

inline bool is_ui_active(UI_System* ui_state, int id)
{
    return ui_state->active.ID && ui_state->active.ID == id;
}


inline bool region_hit(UI_System* ui_state, glm::vec2 pos, glm::vec2 size)
{
    //check if we are inside a ui_object
    ui_state->mouse_pos;

    // bottom left
    if (pos.x - size.x > ui_state->mouse_pos.x) return false;
    if (pos.y - size.y > ui_state->mouse_pos.y) return false;

    // top left
    if (pos.x - size.x > ui_state->mouse_pos.x) return false;
    if (pos.y + size.y < ui_state->mouse_pos.y) return false;

    // top right
    if (pos.x + size.x < ui_state->mouse_pos.x)return false;
    if (pos.y + size.y < ui_state->mouse_pos.y) return false;

    // bottom right
    if (pos.x + size.x < ui_state->mouse_pos.x) return false;
    if (pos.y - size.y > ui_state->mouse_pos.y) return false;

    return true;
}

inline bool region_hit_new(UI_System* ui_state, glm::vec2 pos, glm::vec2 size)
{
    //check if we are inside a ui_object
    ui_state->mouse_pos;

    //printf("DEBUG REGION HIT: MOUSE: %f, %f POS: %f, %f SIZE: %f, %f\n", ui_state->mouse_pos.x, ui_state->mouse_pos.y, pos.x, pos.y, size.x, size.y);

    //top left
    if (pos.x > ui_state->mouse_pos.x) return false;
    if (pos.y > ui_state->mouse_pos.y) return false;


    // bottom left
    if (pos.x > ui_state->mouse_pos.x) return false;
    if (pos.y + size.y < ui_state->mouse_pos.y) return false;


    //top right
    if (pos.x + size.x < ui_state->mouse_pos.x) return false;
    if (pos.y > ui_state->mouse_pos.y) return false;

    // bottom right
    if (pos.x + size.x < ui_state->mouse_pos.x) return false;
    if (pos.y + size.y < ui_state->mouse_pos.y) return false;


    return true;
}


/*
bool button(UI_STATE& ui_state, int id, int x, int y)
{
    if (region_hit())
    {
        ui = hot;
        if (acitve == 0 and mouse down)
            activeitem = id
    }
}*/

//check if we can use the button
inline bool use_button(UI_System* ui_state, UIID id, glm::vec2 pos, glm::vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region
    if (ui_state->mouse_down == 0 &&
        ui_state->active.ID == id.ID &&
        region_hit(ui_state, pos, size))
        return true;

    return false;
}

inline bool use_button_new(UI_System* ui_state, UIID id, glm::vec2 pos, glm::vec2 size)
{
    //checking if we released the mouse button, are active, and we are inside the hit region
    if (ui_state->mouse_down == 0 &&
        ui_state->active.ID == id.ID &&
        region_hit_new(ui_state, pos, size))
        return true;

    return false;
}

inline int generate_id(UI_System* ui_state)
{
    return ui_state->id_generation_number++;
}

inline void set_hot(UI_System* ui_state, UIID id)
{
    if (ui_state->hot.layer <= id.layer)
    {
        ui_state->hot.ID = id.ID;
        ui_state->hot.layer = id.layer;
        //printf("ID: %d, is hot\n", id);
    }
}

inline void set_active(UI_System* ui_state, UIID id)
{
    if (ui_state->active.layer <= id.layer)
    {
        ui_state->active.ID = id.ID;
        ui_state->active.layer = id.layer;
        //printf("ID: %d, is active\n", id);
    }
}

inline bool can_be_active(UI_System* ui_state)
{
    return ui_state->active.ID == -1 && ui_state->mouse_down;
}

inline bool is_active(UI_System* ui_state, UIID id)
{
    return ui_state->active.ID == id.ID;
}

inline bool is_hot(UI_System* ui_state, UIID id)
{
    return ui_state->hot.ID == id.ID;
}


inline bool do_button(UI_System* ui_state, UIID id, glm::vec2 pos, glm::vec2 screen_percentage,
                      glm::vec3 color = {1.0f, 1.0f, 1.0f}, glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                      pressed_color = {1.0f, 1.0f, 1.0f})
{
    /*validation*/

    if (screen_percentage.x > 100 || screen_percentage.x < 0 || screen_percentage.y > 100 || screen_percentage.y < 0)
    {
        printf("SCREEN PERCENTAGE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    /*POS AND SIZE CALCULATIONS*/

    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = screen_percentage / 100.0f;

    glm::vec2 final_pos = {
        ui_state->push_constants.screenSize.x * converted_pos.x,
        ui_state->push_constants.screenSize.y * converted_pos.y
    };
    glm::vec2 final_size = {
        (ui_state->push_constants.screenSize.x * converted_size.x) / 2,
        (ui_state->push_constants.screenSize.y * converted_size.y) / 2
    };


    /*SET UI STATE*/

    int mesh_id = ui_state->draw_info.vertex_info.mesh_id++;

    //check if button is hot and active
    if (region_hit(ui_state, final_pos, final_size))
    {
        set_hot(ui_state, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_state))
        {
            set_active(ui_state, id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    /*SET DRAW COLOR Based on the state*/
    std::vector<Vertex> new_quad;

    //set color
    //active state
    if (is_active(ui_state, id))
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_state, id))
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, color);
    }


    /*SET DRAW INFO*/

    uint16_t base_index = static_cast<uint16_t>(ui_state->draw_info.vertex_info.dynamic_vertices.size());

    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };

    // Add vertices
    ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());
    // Add indices
    ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());

    //check if we clicked the button
    if (use_button(ui_state, id, final_pos, final_size)) return true;

    return false;
}


inline bool do_button_new(UI_System* ui_state, UIID id, glm::vec2 pos, glm::vec2 size,
                          Alignment alignment = Alignment::LEFT, glm::vec3 color = {1.0f, 1.0f, 1.0f},
                          glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                          pressed_color = {1.0f, 1.0f, 1.0f})
{
    /*validation*/
    if (size.x > 100 || size.x < 0 || size.y > 100 || size.y < 0)
    {
        printf("SIZE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }


    /*POS AND SIZE CALCULATIONS*/

    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = size / 100.0f;

    //TODO: this realistically can be moved out to the ui_state struct
    //TODO: THIS IS COMPLETELY WRONG and does not work how i think it does
    float quarter_size_x = ui_state->push_constants.screenSize.x * 0.25;
    float quarter_size_y = ui_state->push_constants.screenSize.y * 0.25;
    float half_size_x = ui_state->push_constants.screenSize.x * 0.5;
    float half_size_y = ui_state->push_constants.screenSize.y * 0.5;
    glm::vec2 final_pos;
    switch (alignment)
    {
        case Alignment::CENTER:
            final_pos = {
                ui_state->push_constants.screenSize.x * converted_pos.x - quarter_size_x,
                ui_state->push_constants.screenSize.y * converted_pos.y - quarter_size_y,
            };
            break;
        case Alignment::LEFT:
            final_pos = {
                ui_state->push_constants.screenSize.x * converted_pos.x,
                ui_state->push_constants.screenSize.y * converted_pos.y,
            };
            break;
        case Alignment::RIGHT:
            final_pos = {
                ui_state->push_constants.screenSize.x * converted_pos.x + half_size_x,
                ui_state->push_constants.screenSize.y * converted_pos.y,
            };
            break;
    }


    glm::vec2 final_size = {
        (ui_state->push_constants.screenSize.x * converted_size.x),
        (ui_state->push_constants.screenSize.y * converted_size.y),
    };

    /*SET UI STATE*/

    int mesh_id = ui_state->draw_info.vertex_info.mesh_id++;

    //check if button is hot and active
    if (region_hit_new(ui_state, final_pos, final_size))
    {
        set_hot(ui_state, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_state))
        {
            set_active(ui_state, id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    /*SET DRAW COLOR Based on the state*/
    std::vector<Vertex> new_quad;

    //set color
    //active state
    if (is_active(ui_state, id))
    {
        new_quad = UI_create_quad(final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_state, id))
    {
        new_quad = UI_create_quad(final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad(final_pos, final_size, color);
    }


    /*SET DRAW INFO*/

    uint16_t base_index = static_cast<uint16_t>(ui_state->draw_info.vertex_info.dynamic_vertices.size());

    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };

    // Add vertices
    ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());
    // Add indices
    ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());

    //check if we clicked the button
    if (use_button_new(ui_state, id, final_pos, final_size)) return true;

    return false;
}

inline bool do_button_new_text(UI_System* ui_state, UIID id, glm::vec2 pos, glm::vec2 size, std::string text,
                               glm::vec2 text_padding = {0.0f, 0.0f}, glm::vec3 color = {1.0f, 1.0f, 1.0f},
                               glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                               pressed_color = {1.0f, 1.0f, 1.0f})
{
    /*validation*/
    if (size.x > 100 || size.x < 0 || size.y > 100 || size.y < 0)
    {
        printf("SIZE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }


    /*POS AND SIZE CALCULATIONS*/

    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = size / 100.0f;

    glm::vec2 final_pos = {
        ui_state->push_constants.screenSize.x * converted_pos.x,
        ui_state->push_constants.screenSize.y * converted_pos.y,
    };

    glm::vec2 final_size = {
        (ui_state->push_constants.screenSize.x * converted_size.x),
        (ui_state->push_constants.screenSize.y * converted_size.y),
    };

    do_text(ui_state, std::move(text), pos + text_padding, glm::vec3{1.0, 1.0, 0.0});


    /*SET UI STATE*/

    int mesh_id = ui_state->draw_info.vertex_info.mesh_id++;

    //check if button is hot and active
    if (region_hit_new(ui_state, final_pos, final_size))
    {
        set_hot(ui_state, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_state))
        {
            set_active(ui_state, id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    /*SET DRAW COLOR Based on the state*/
    std::vector<Vertex> new_quad;

    //set color
    //active state
    if (is_active(ui_state, id))
    {
        new_quad = UI_create_quad(final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_state, id))
    {
        new_quad = UI_create_quad(final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad(final_pos, final_size, color);
    }


    /*SET DRAW INFO*/

    uint16_t base_index = static_cast<uint16_t>(ui_state->draw_info.vertex_info.dynamic_vertices.size());

    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };

    // Add vertices
    ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());
    // Add indices
    ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());

    //check if we clicked the button
    if (use_button_new(ui_state, id, final_pos, final_size)) return true;

    return false;
}

inline bool do_button_text(UI_System* ui_state, UIID id, std::string text, glm::vec2 pos, glm::vec2 screen_percentage,
                           glm::vec3 color = {1.0f, 1.0f, 1.0f}, glm::vec3 hovered_color = {1.0f, 1.0f, 1.0f}, glm::vec3
                           pressed_color = {1.0f, 1.0f, 1.0f})
{
    /*validation*/

    if (screen_percentage.x > 100 || screen_percentage.x < 0 || screen_percentage.y > 100 || screen_percentage.y < 0)
    {
        printf("SCREEN PERCENTAGE CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }
    if (pos.x > 100 || pos.x < 0 || pos.y > 100 || pos.y < 0)
    {
        printf("POSITION CANNOT BE GREATER THAN 100 AND LESS THAN 0");
    }

    /*POS AND SIZE CALCULATIONS*/

    glm::vec2 converted_pos = pos / 100.0f;
    glm::vec2 converted_size = screen_percentage / 100.0f;

    glm::vec2 final_pos = {
        ui_state->push_constants.screenSize.x * converted_pos.x,
        ui_state->push_constants.screenSize.y * converted_pos.y
    };
    glm::vec2 final_size = {
        (ui_state->push_constants.screenSize.x * converted_size.x) / 2,
        (ui_state->push_constants.screenSize.y * converted_size.y) / 2
    };

    //do_text(ui_state, text, final_pos, {1.0f,1.0f,1.0f});
    do_text_screen_percentage(ui_state, text, pos, screen_percentage, {0.0f, 0.0f, 1.0f}, 48);


    /*SET UI STATE*/

    int mesh_id = ui_state->draw_info.vertex_info.mesh_id++;

    //check if button is hot and active
    if (region_hit(ui_state, final_pos, final_size))
    {
        set_hot(ui_state, id);

        //check if we have the mouse pressed and nothing else is selected
        //TODO: so there is a bug with can_be_active, in that the first ui called on the screen will take active focus,
        //TODO: this is despite there bieng another ui in front of it
        //TODO: for now imma just leave it be and dont draw things on top of others
        if (can_be_active(ui_state))
        {
            set_active(ui_state, id);
        }
    }
    /*
    else
    {
        printf("is not hot\n");
    }*/


    /*SET DRAW COLOR Based on the state*/
    std::vector<Vertex> new_quad;

    //set color
    //active state
    if (is_active(ui_state, id))
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, pressed_color);
    }
    //hot state
    else if (is_hot(ui_state, id))
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, hovered_color);
    }
    // normal state
    else
    {
        new_quad = UI_create_quad_screen_percentage(final_pos, final_size, color);
    }


    /*SET DRAW INFO*/

    uint16_t base_index = static_cast<uint16_t>(ui_state->draw_info.vertex_info.dynamic_vertices.size());

    // create indices (two triangles per quad)
    std::vector<uint16_t> quad_indices = {
        static_cast<uint16_t>(base_index + 0),
        static_cast<uint16_t>(base_index + 1),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 2),
        static_cast<uint16_t>(base_index + 3),
        static_cast<uint16_t>(base_index + 0)
    };

    // Add vertices
    ui_state->draw_info.vertex_info.dynamic_vertices.insert(ui_state->draw_info.vertex_info.dynamic_vertices.end(),
                                                            new_quad.begin(),
                                                            new_quad.end());
    // Add indices
    ui_state->draw_info.vertex_info.dynamic_indices.insert(ui_state->draw_info.vertex_info.dynamic_indices.end(),
                                                           quad_indices.begin(), quad_indices.end());

    //check if we clicked the button
    if (use_button(ui_state, id, final_pos, final_size)) return true;

    return false;
}

inline void ui_begin(UI_System* ui_state)
{
    //clear draw info and reset the hot id

    ui_state->draw_info.vertex_info.vertex_buffer_should_update = true;
    //std::cout << "MOUSE STATE:" << ui_state.mouse_down << '\n';

    clear_vertex_info(ui_state->draw_info.vertex_info);
    ui_state->draw_info.UI_Objects.clear();

    ui_state->hot.ID = -1;
    ui_state->hot.layer = -1;


    //UPDATE TEXT
    text_update(ui_state->text_system, ui_state->push_constants);
}

inline void ui_end(UI_System* ui_state, GLFWwindow* window)
{
    //check if mouse is released, if so reset the active id
    //also update the mouse state

    //printf("HOT ID: %d, HOT LAYER: %d\n", ui_state->hot.ID, ui_state->hot.layer);
    //printf("ACTIVE ID: %d, ACTIVE LAYER: %d\n", ui_state->active.ID, ui_state->active.layer);

    if (ui_state->mouse_down == GLFW_RELEASE)
    {
        //std::cout << "mouse released\n";
        ui_state->active.ID = -1;
    }


    //update mouse state
    ui_state->mouse_down = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    //update mouse pos
    ui_state->mouse_pos = get_mouse_pos(window);
}


#endif //UI_H
