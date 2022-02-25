#include "app.h"

void app::start()
{
    if (!glfwInit()) {
        throw std::runtime_error("Could not initialise GLFW");
    }

    // Set required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
}


void app::stop()
{
    if (m_main_window)
        glfwDestroyWindow(m_main_window);

    if (m_surface)
        cairo_surface_destroy(m_surface);
}


/**
 * Show the main window
 */
void app::show_main_window()
{
    m_main_window = glfwCreateWindow(m_width, m_height, OMI75C_FULL_NAME, nullptr, nullptr);

    if (!m_main_window)
        throw std::runtime_error("Could not create main window");

    glfwMakeContextCurrent(m_main_window);

    glViewport(0, 0, m_width, m_height);

    if (GLEW_OK != glewInit())
        throw std::runtime_error("Failed to initialize GLEW");

    // Generate a texture to work with
    glGenTextures(1, &m_texture_id);
    glBindTexture(GL_TEXTURE_2D, m_texture_id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);

    m_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, m_width, m_height);
    if (!m_surface)
        throw std::runtime_error("Could not create surface");

    m_cr = cairo_create(m_surface);
}


void app::event_loop()
{
    while (!glfwWindowShouldClose(m_main_window)) {
        // check if any events have been activated (mouse, keyboard, ...)
        glfwPollEvents();

        render_main_window();

        glfwSwapBuffers(m_main_window);
    }
}


void app::render_main_window()
{
    int width;
    int height;

    glfwGetFramebufferSize(m_main_window, &width, &height);

    // set red background - or at least try to :-)
    cairo_set_source_rgb(m_cr, 1.0, 0.0, 0);
    cairo_paint(m_cr);

    void *data = cairo_image_surface_get_data(m_surface);
    cairo_surface_flush(m_surface);

    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, m_texture_id);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, m_width, m_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 1);
    glVertex2i(0, 0);
    glTexCoord2f(0, 0);
    glVertex2i(0, m_height);
    glTexCoord2f(1, 0);
    glVertex2i(m_width, m_height);
    glTexCoord2f(1, 1);
    glVertex2i(m_width, 0);
    glEnd();

    glBindTexture(GL_TEXTURE_2D, 0);
}
