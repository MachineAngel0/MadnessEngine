#ifndef UI_3D_H
#define UI_3D_H

// What makes 3d ui different from 2d, and features needed
// 1. they are functionally the same, meaning they should share the same code, except that 3d UI has a z position in the world
// 2. the ui needs to have an attachable transform to other objects, like in the instance of displaying enemy health
// 3. the ui needs the option to be either a billboard type or just a 3d object with on backface (which can just be a bool type in the shader)
// 4. the doesn't need to be clickable or anything fancy, just needs to display gamestate information dynamically


struct UI_3D
{
    //UI_2D whatever it is
    //transform (parent object) (we do not consider the ui's children/parent to be part of the transform )
    //3d_type (normal, billboard)
}UI_3D;



#endif //UI_3D_H
