lambda = 2. * Math.PI
period = 2. * Math.PI
c = 299792458.0

// reference_frequency = 2 * Math.PI * c / 800e-9

control = {
    total_particle_number: 1,
    time_step: 0.001 * period,
    data_start_time: 30 * period,
    data_end_time: 100 * period,
    data_interval: 50,
    data_per_frame: 30,
    frames_per_second: 25,
}

field = {
    name: "LaguerreGaussianPulseField",
    a0: 5,
    tau: 5 * period,
    w0: 5 * lambda,
    delay: 50 * period
}

particle_producer = {
    particle: {
        type: 1,
        position: [0.,0.,0.],
        momentum: [0.,0.,0.]
    }
}

algorithm = {
    name: "LeapfrogAlgorithm"
}