c = 299792458.0
mass = 1836.
charge = 1.

let U = 2000 / 0.511e6;
let L = 0.0005;
let delay = 0.25;

// reference_frequency = 2 * Math.PI * c / 800e-9

control = {
    total_particle_number: 1,
    time_step: 0.0001,
    data_start_time: 0.,
    data_end_time: 20,
    data_interval: 100,
    data_per_frame: 30,
    frames_per_second: 25,
}

field = {
    name: "CustomField",
    func: (x,y,z,t) => {
        return {
            E:[x < L/2 && x > -L/2 ? U/L*Math.sin(2*Math.PI*(t+delay)) : 0,0,0],
            B:[0,0,2*Math.PI*mass/charge]
        }
    }
}

particle_producer = {
    particle: {
        mass: mass,
        charge: charge,
        position: [0.,0.,0.],
        momentum: [0.,0.,0.]
    }
}


algorithm = {
    name: "LeapfrogAlgorithm"
}