lambda = 2. * math.pi
period = 2. * math.pi
beam_width = 1. * lambda
beam_length = 3. * lambda
kinetic_energy = 500
energy_spread = 0.05
angular_divergence = 0.01

function init_position()
    return {
        C_normal_distribution(0., beam_width/4.),
        C_normal_distribution(0., beam_width/4.),
        C_random(-beam_length/2., beam_length/2.)
    }
end

function init_momentum()
    local kinetic_energy_stddev = kinetic_energy * energy_spread / (2. * math.sqrt(2. * math.log(2.)))
    local Ek = C_normal_distribution(kinetic_energy, kinetic_energy_stddev)
    while Ek < 0. do Ek = C_normal_distribution(kinetic_energy, kinetic_energy_stddev) end

    local p = math.sqrt((Ek + mass)^2 - mass^2);
    local theta_x = C_normal_distribution(0., angular_divergence/4.)
    local theta_y = C_normal_distribution(0., angular_divergence/4.)
    local theta = math.sqrt(theta_x^2 + theta_y^2);
    local phi = math.atan(theta_y, theta_x)
    return {
        p * math.sin(theta) * math.cos(phi),
        p * math.sin(theta) * math.sin(phi),
        p * math.cos(theta)
    }
end

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
                    center = {0, 0, 2},
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
            global_function_name = "custom_field"
        }
    }
}

particle = {
    type = 1, -- 0: normal, 1: electron
    -- mass = 1,
    -- charge = -1,
    position = init_position,
    momentum = init_momentum
}

particle_shifters = {
}