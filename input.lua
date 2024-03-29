lambda = 2. * math.pi
period = 2. * math.pi
c = 299792458.0
beam_width = 1. * lambda -- 束流宽度，沿任意横向方向测量该宽度之内的粒子数为95%（4个标准差），正态分布
beam_length = 3. * lambda -- 束流纵向长度，均匀分布
kinetic_energy = 500 -- 粒子束平均动能
energy_spread = 0.05 -- 粒子束能散，流强半高全宽定义下的能量分散程度，正态分布
angular_divergence = 0.01 -- 粒子束角度发散，沿任意横向方向测量，该角度之内的粒子数为95%（仅限小角度，即可做近似tanθ=θ的情况），正态分布
spin_kappa = 10 -- 球面Fisher分布中的kappa系数，越小则分布越集中于[0,0,1]

function init_position()
    return {
        C_normal_distribution(0., beam_width/4.),
        C_normal_distribution(0., beam_width/4.),
        C_random(-beam_length/2., beam_length/2.)
    }
end

function init_momentum(mass)
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

function custom_particle_shifter(particle)
    return {
        position = init_position(),
        momentum = init_momentum(particle.mass),
        polarization = C_fisher_distribution(spin_kappa)
    }
end

function custom_field(x,y,z,t)
    return {
        E = {1,0,0},
        B = {0,1,0}
    }
end

reference_frequency = 2 * math.pi * c / 800e-9

control = {
    total_particle_number = 100,
    time_step = 0.001 * period,
    data_start_time = 0. * period,
    data_end_time = 1. * period,
    data_interval = 200,
    parallel_workers = 8,
    table_dirname = "./tables",
    output_filename = "test.h5",
    output_items = {"x","y","z","px","py","pz","sx","sy","sz","Ex","Ey","Ez","Bx","By","Bz","optical_depth"}
}

field = {
    name = "FieldCombiner",
    fields = {
        {
            name = "FieldWithShifters",
            field = {
                name = "LaguerreGaussianPulseField",
                a0 = 5,
                tau = 12 * 2 * math.pi,
                w0 = 6 * 2 * math.pi,
                -- polar = 0.,
                -- delay = 0.,
                -- p = 0,
                -- l = 0,
                -- iphase = 0.,
                -- k = 1.,
                -- h = 0.1,
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

particle_producer = {
    particle = {
        type = 1, -- 0: normal, 1: electron
        -- mass = 1,
        -- charge = -1,
        position = {0.,0.,0.},
        momentum = {0.,0.,0.}
    },
    shifters = {
        {
            name = "CustomParticleShifter",
            global_function_name = "custom_particle_shifter"
        },
        {
            name = "ParticleTranslator",
            displacement = {3, 0, 0}
        },
        {
            name = "ParticleRotator",
            -- center = {0,0,0},
            rotator = {
                name = "ExtrinsicRotator",
                alpha = math.pi / 2,
                beta = math.pi / 2,
                gamma = 0,
            },
            affect_position = false,
            affect_momentum = false,
            -- affect_polarization = true,
        }
    }
}


algorithm = {
    name = "MonteCarloRadiativePolarizationAlgorithm",
    base_algorithm = {
        name = "ModifiedAMMLeapfrogAlgorithm"
    },
    min_chi_e = 1e-5,
    disable_reaction = true,
    -- disable_spin_effect = false,
}