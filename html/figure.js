
class Figure {
  constructor(plot, id, options) {
    this.plot = plot;
    this.id = id;
    this.plot.setOption({
      animation: false,
      backgroundColor: '#fff'
    })
  }

  static getLabel(variable) {
    return `${variable.name}${variable.unit ? ` [${variable.unit}]` : ''}`
  }

  updateFrame() {
    throw new Error("Not implement.");
  }

  clear() {
    throw new Error("Not implement.");
  }

  addData(data) {
    throw new Error("Not implement.");
  }
}

Figure.variables = {
  x: {
    name: "x",
    unit: "c/ω",
    processData: (data) => data.particle.position[0],
  },
  y: {
    name: "y",
    unit: "c/ω",
    processData: (data) => data.particle.position[1],
  },
  z: {
    name: "z",
    unit: "c/ω",
    processData: (data) => data.particle.position[2],
  },
  px: {
    name: "px",
    unit: "m_e c",
    processData: (data) => data.particle.momentum[0],
  },
  py: {
    name: "py",
    unit: "m_e c",
    processData: (data) => data.particle.momentum[1],
  },
  pz: {
    name: "pz",
    unit: "m_e c",
    processData: (data) => data.particle.momentum[2],
  },
  sx: {
    name: "sx",
    processData: (data) => data.particle.polarization[0],
  },
  sy: {
    name: "sy",
    processData: (data) => data.particle.polarization[1],
  },
  sz: {
    name: "sz",
    processData: (data) => data.particle.polarization[2],
  },
  Ex: {
    name: "Ex",
    unit: "m_e cω/e",
    processData: (data) => data.particle.em_field.E[0],
  },
  Ey: {
    name: "Ey",
    unit: "m_e cω/e",
    processData: (data) => data.particle.em_field.E[1],
  },
  Ez: {
    name: "Ez",
    unit: "m_e cω/e",
    processData: (data) => data.particle.em_field.E[2],
  },
  Bx: {
    name: "Bx",
    unit: "m_e ω/e",
    processData: (data) => data.particle.em_field.B[0],
  },
  By: {
    name: "By",
    unit: "m_e ω/e",
    processData: (data) => data.particle.em_field.B[1],
  },
  Bz: {
    name: "Bz",
    unit: "m_e ω/e",
    processData: (data) => data.particle.em_field.B[2],
  },
  Ek: {
    name: "Ek",
    unit: "m_e c2",
    processData: (data) => Math.sqrt(
      Math.pow(data.particle.mass, 2)
      + Math.pow(data.particle.momentum[0], 2)
      + Math.pow(data.particle.momentum[1], 2)
      + Math.pow(data.particle.momentum[2], 2)
    ) - data.particle.mass,
  },
  t: {
    name: "t",
    unit: "1/ω",
    processData: (data) => data.time,
  }
};

class FigureLine extends Figure {
  constructor(plot, id, options) {
    super(plot, id, options)
    this.series = [];
  }

  updateFrame() {
    this.plot.setOption({ series: this.series });
  }

  clear() {
    for (let i = 0; i < this.series.length; i++) {
      this.series[i] = this.getSeriesElement();
    }
    this.updateFrame();
    this.series = []
  }

  getSeriesElement() {
    throw new Error("Not implement.");
  }
}

class Figure2dLine extends FigureLine {
  constructor(plot, id, options) {
    super(plot, id, options);
    this.x = Figure.variables[options.xAxis];
    this.y = Figure.variables[options.yAxis];
    this.plot.setOption({
      xAxis: {
        type: 'value',
        name: Figure.getLabel(this.x),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      yAxis: {
        type: 'value',
        name: Figure.getLabel(this.y),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      }
    });
  }

  getSeriesElement() {
    return {
      type: 'line',
      symbol: 'none',
      data: [],
      lineStyle: {
        width: 2
      }
    };
  }

  addData(data) {
    if (data.type == "new_particle") {
      this.series.push(this.getSeriesElement());
    } else if (data.type == "data") {
      const x = this.x.processData(data);
      const y = this.y.processData(data);
      this.series[this.series.length - 1].data.push([x, y]);
    } else if (data.type == "end_particle" || data.type == "update_frame") {
      this.updateFrame()
    }
  }
}


class Figure3dLine extends FigureLine {
  constructor(plot, id, options) {
    super(plot, id, options);
    this.x = Figure.variables[options.xAxis];
    this.y = Figure.variables[options.yAxis];
    this.z = Figure.variables[options.zAxis];
    this.plot.setOption({
      xAxis3D: {
        type: 'value',
        name: Figure.getLabel(this.x),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      yAxis3D: {
        type: 'value',
        name: Figure.getLabel(this.y),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      zAxis3D: {
        type: 'value',
        name: Figure.getLabel(this.z),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      grid3D: {
        viewControl: {
          projection: 'orthographic'
        }
      }
    });
  }

  getSeriesElement() {
    return {
      type: 'line3D',
      data: [],
      lineStyle: {
        width: 2
      }
    };
  }

  addData(data) {
    if (data.type == "new_particle") {
      this.series.push(this.getSeriesElement());
    } else if (data.type == "data") {
      const x = this.x.processData(data);
      const y = this.y.processData(data);
      const z = this.z.processData(data);
      this.series[this.series.length - 1].data.push([x, y, z]);
    } else if (data.type == "end_particle" || data.type == "update_frame") {
      this.updateFrame()
    }
  }
}

class FigureScatter extends Figure {

  updateFrame() {
    this.plot.setOption({
      visualMap: [{
        min: typeof this.min !== 'undefined' ? this.min : 0,
        max: typeof this.max !== 'undefined' ? this.max : 200
      }],
      title: {
        left: 'center',
        text: `${this.c.name}:[${typeof this.min !== 'undefined'
          && this.min.toExponential(2)},${typeof this.max !== 'undefined'
          && this.max.toExponential(2)}]`
      },
    })
    this.plot.resize()
  }

  clear() {
    this.min = undefined
    this.max = undefined
    this.plot.setOption({ series: [{ data: [] }] })
    this.updateFrame();
  }
}

class Figure2dScatter extends FigureScatter {
  constructor(plot, id, options) {
    super(plot, id, options);
    this.x = Figure.variables[options.xAxis]
    this.y = Figure.variables[options.yAxis]
    this.c = Figure.variables[options.cAxis]
    this.plot.setOption({
      visualMap: [{
        text: [Figure.getLabel(this.c)],
        right: 10,
        top: 'center',
        inRange: {
          color: options.color ? eval(options.color) : ['rgba(0,0,255,1)', 'rgba(255,0,0,1)']
        }
      }],
      title: {
        left: 'center',
        text: `${this.c.name}:[NaN,NaN]`
      },
      xAxis: {
        type: 'value',
        name: Figure.getLabel(this.x),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      yAxis: {
        type: 'value',
        name: Figure.getLabel(this.y),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      series: [{
        type: 'scatter',
        //symbolSize: 1,
        data: [],
        large: true,
        largeThreshold: 2000
      }]
    });
  }

  addData(data) {
    if (data.type == "end_particle") {
      const x = this.x.processData(data);
      const y = this.y.processData(data);
      const c = this.c.processData(data);
      if (typeof this.max === 'undefined' || c > this.max) {
        this.max = c;
      }
      if (typeof this.min === 'undefined' || c < this.min) {
        this.min = c;
      }
      this.plot.appendData({ seriesIndex: 0, data: [[x, y, c]] })
      this.updateFrame();
    }
  }
}

class Figure3dScatter extends FigureScatter {
  constructor(plot, id, options) {
    super(plot, id, options);
    this.x = Figure.variables[options.xAxis]
    this.y = Figure.variables[options.yAxis]
    this.z = Figure.variables[options.zAxis]
    this.c = Figure.variables[options.cAxis]
    this.plot.setOption({
      visualMap: [{
        text: [Figure.getLabel(this.c)],
        right: 10,
        top: 'center',
        inRange: {
          color: options.color ? eval(options.color) : ['rgba(0,0,255,1)', 'rgba(255,0,0,1)']
        }
      }],
      title: {
        left: 'center',
        text: `${this.c.name}:[NaN,NaN]`
      },
      xAxis3D: {
        type: 'value',
        name: Figure.getLabel(this.x),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      yAxis3D: {
        type: 'value',
        name: Figure.getLabel(this.y),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      zAxis3D: {
        type: 'value',
        name: Figure.getLabel(this.z),
        nameLocation: 'center',
        axisLine: {
          onZero: false
        }
      },
      grid3D: {
        viewControl: {
          projection: 'orthographic'
        }
      },
      series: [{
        type: 'scatter3D',
        //symbolSize: 1,
        data: []
      }]
    });
  }

  addData(data) {
    if (data.type == "end_particle") {
      const x = this.x.processData(data);
      const y = this.y.processData(data);
      const z = this.z.processData(data);
      const c = this.c.processData(data);
      if (typeof this.max === 'undefined' || c > this.max) {
        this.max = c;
      }
      if (typeof this.min === 'undefined' || c < this.min) {
        this.min = c;
      }
      this.plot.appendData({ seriesIndex: 0, data: [[x, y, z, c]] })
      this.updateFrame();
    }
  }
}
