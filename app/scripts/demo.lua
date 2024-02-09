entities = {
    {
        name = "Guy",
        modelFile = "models/LowPolyCharacter.obj",
        components = {
            RenderComponent = { color = {0, 0, 0} },
            TransformComponent = { position = {1, -1, 0} },
            MoveComponent = {},
            RotateComponent = {}
        }
    },
    {
        name = "Floor",
        modelFile = "models/quad.obj",
        components = {
            RenderComponent = { color = {0, 0, 0} },
            TransformComponent = { position = {0.5, 0.5, 0}, scale = {10, 1, 10} },
            MoveComponent = {},
            RotateComponent = {}
        }
    },
    {
        name = "Smooth Vase",
        modelFile = "models/smooth_vase.obj",
        components = {
            RenderComponent = { color = {0, 0, 0} },
            TransformComponent = { position = {0.5, -0, 0} },
            MoveComponent = {},
            RotateComponent = {}
        }
    },
    {
        name = "Red Light",
        components = {
            MoveComponent = {},
            TransformComponent = { position = {1, -2, -1}, scale = {0.2, 0.2, 0.2} },
            PointLightComponent = { color = {1.0, 0.2, 0.2, 2} }
        }
    },
    {
        name = "Green Light",
        components = {
            MoveComponent = {},
            TransformComponent = { position = {0, -2, 1}, scale = {0.2, 0.2, 0.2} },
            PointLightComponent = { color = {0.2, 1.0, 0.2, 2} }
        }
    },
    {
        name = "Blue Light",
        components = {
            MoveComponent = {},
            TransformComponent = { position = {-1, -2, -1}, scale = {0.2, 0.2, 0.2} },
            PointLightComponent = { color = {0.2, 0.2, 1.0, 2} }
        }
    }
}