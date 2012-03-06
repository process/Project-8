class Graphics
{
private:
    HWND hwnd; //Handle to the window
    HDC hDC; //Handle to the device context
    HGLRC hRC; //Handle to the rendering context

public:

    Graphics(); // Default constructor

    //Initialize OpenGL for rendering 2D graphics
    void initGraphics();

    //Begin new frame
    void startDraw();

    //Clear screen with selected color
    void clear(int r, int g, int b);

    //Add a pixel to the given position
    void putPixel(int x, int y, int r, int g, int b);

    //Finalize added pixels and render the frame
    void endDraw();

    //Resize the rendering context used by GL
    //Called when windows is resized
    void resizeViewport(int width, int height);

    //Release all memory and contexts used by GL
    void killGL();
};
