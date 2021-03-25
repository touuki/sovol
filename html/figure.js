
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
    processData: (data) => data.pos.x,
  },
  y: {
    name: "y",
    unit: "c/ω",
    processData: (data) => data.pos.y,
  },
  z: {
    name: "z",
    unit: "c/ω",
    processData: (data) => data.pos.z,
  },
  px: {
    name: "px",
    unit: "m_e c",
    processData: (data) => data.mom.x,
  },
  py: {
    name: "py",
    unit: "m_e c",
    processData: (data) => data.mom.y,
  },
  pz: {
    name: "pz",
    unit: "m_e c",
    processData: (data) => data.mom.z,
  },
  Ex: {
    name: "Ex",
    unit: "m_e cω/e",
    processData: (data) => data.E.x,
  },
  Ey: {
    name: "Ey",
    unit: "m_e cω/e",
    processData: (data) => data.E.y,
  },
  Ez: {
    name: "Ez",
    unit: "m_e cω/e",
    processData: (data) => data.E.z,
  },
  Bx: {
    name: "Bx",
    unit: "m_e ω/e",
    processData: (data) => data.B.x,
  },
  By: {
    name: "By",
    unit: "m_e ω/e",
    processData: (data) => data.B.y,
  },
  Bz: {
    name: "Bz",
    unit: "m_e ω/e",
    processData: (data) => data.B.z,
  },
  Ek: {
    name: "Ek",
    unit: "m_e c2",
    processData: (data) => data.Ek,
  },
  thetax: {
    name: "θx",
    unit: "m_e c",
    processData: (data) => - data.mom.x/data.mom.z,
  },
  thetay: {
    name: "θy",
    unit: "m_e c",
    processData: (data) => - data.mom.y/data.mom.z,
  },
  t: {
    name: "t",
    unit: "1/ω",
    processData: (data) => data.t,
  }
};

class FigureLine extends Figure {
  constructor(plot, id, options) {
    super(plot, id, options)
    this.series = []
  }

  updateFrame() {
    this.plot.setOption({ series: this.series })
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
    if (data.isNewParticle) {
      this.series.push(this.getSeriesElement());
    }
    const x = this.x.processData(data.data);
    const y = this.y.processData(data.data);
    this.series[this.series.length - 1].data.push([x, y]);
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
    if (data.isNewParticle) {
      this.series.push(this.getSeriesElement());
    }
    const x = this.x.processData(data.data);
    const y = this.y.processData(data.data);
    const z = this.z.processData(data.data);
    this.series[this.series.length - 1].data.push([x, y, z]);
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
    if (data.endtimeReached) {
      const x = this.x.processData(data.data);
      const y = this.y.processData(data.data);
      const c = this.c.processData(data.data);
      if (typeof this.max === 'undefined' || c > this.max) {
        this.max = c;
      }
      if (typeof this.min === 'undefined' || c < this.min) {
        this.min = c;
      }
      this.plot.appendData({ seriesIndex: 0, data: [[x, y, c]] })
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
    if (data.endtimeReached) {
      const x = this.x.processData(data.data);
      const y = this.y.processData(data.data);
      const z = this.z.processData(data.data);
      const c = this.c.processData(data.data);
      if (typeof this.max === 'undefined' || c > this.max) {
        this.max = c;
      }
      if (typeof this.min === 'undefined' || c < this.min) {
        this.min = c;
      }
      this.plot.appendData({ seriesIndex: 0, data: [[x, y, z, c]] })
    }
  }
}
