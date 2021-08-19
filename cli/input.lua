function custom_field(x,y,z,t)
    return {
        E = {1,0,0},
        B = {0,1,0}
    }
end

field = {
    name = "FieldCombiner",
    fields = {
        {
            name = "FieldWithShifters",
            field = {
                name = "LaguerreGaussianPulseField",
                a0 = 5,
                tau = 12 * 2 * math.pi,
            },
            shifters = {
                {
                    name = "FieldRotator",
                    center = {0, 0, 2}
                    rotator = {
                        name = "IntrinsicRotator",
                        alpha = math.pi / 2,
                        beta = math.pi / 2,
                        gamma = 0,
                    }
                },
                {
                    name = "FieldTranslator",
                    displacement = {3, 3, 0}
                }
            }
        },
        {
            name = "CustomField",
            function_name = "custom_field"
        }
    }
}