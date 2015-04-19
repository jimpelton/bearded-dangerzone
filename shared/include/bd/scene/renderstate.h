#ifndef renderstate_h__
#define renderstate_h__


namespace bd {

class VertexArrayObject;
class ShaderProgram;

//////////////////////////////////////////////////////////////////////////
// \brief Applies states of VertexArrayObject and ShaderPrograms and
// calls glDraw*() methods.
//
// The GL render mode used is GL_TRIANGLE_STRIP.
//////////////////////////////////////////////////////////////////////////

class RenderState
{
public:
    //////////////////////////////////////////////////////////////////////////
    /// \brief Create a RenderState object that draws with \c method.
    //////////////////////////////////////////////////////////////////////////
    RenderState(VertexArrayObject *, ShaderProgram *);

    ~RenderState();

    //////////////////////////////////////////////////////////////////////////
    /// \brief Draw using the provided VertexArrayObject and ShaderProgram.
    //////////////////////////////////////////////////////////////////////////
    void draw();

private:
    void drawElementArrays();
    void drawArrays();

    VertexArrayObject *m_vao;
    ShaderProgram *m_progy;

};


} // namespace bd

#endif // !renderstate_h__

