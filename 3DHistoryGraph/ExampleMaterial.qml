import QtQuick 2.9
import QtQuick3D 1.14
import QtQuick3D.Materials 1.14

CustomMaterial {
    property real time: 0.0
    property real amplitude: 5.0

    shaderInfo: ShaderInfo {
        version: "330"
        type: "GLSL"
    }

    Shader {
        id: vertShader
        stage: Shader.Vertex
        shader: "example2.vert"
    }

    Shader {
        id: fragShader
        stage: Shader.Fragment
        shader: "example.frag"
    }

    passes: [ Pass {
            shaders: [ vertShader, fragShader ]
        }
    ]
}
