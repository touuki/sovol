
class Figure {
  constructor(plot, id, axes) {
    this.plot = plot;
    this.id = id;
    this.initSeries();
  }

  initSeries() {
    this.series = [];
  }

  updateFrame() {
    this.plot.setOption({ series: this.series })
  }

  clear() {
    for (let i = 0; i < this.series.length; i++) {
      this.series[i] = this.getSeriesElement();
    }
    this.updateFrame();
    this.initSeries();
  }

  getSeriesElement() {
    throw new Error("Not implement.");
  }

  addData(data, isNewParticle) {
    throw new Error("Not implement.");
  }
}

Figure.variables = {
  x: {
    name: "x",
    unit: "c/ω",
    processData: (data) => data.x,
  },
  y: {
    name: "y",
    unit: "c/ω",
    processData: (data) => data.y,
  },
  z: {
    name: "z",
    unit: "c/ω",
    processData: (data) => data.z,
  },
  px: {
    name: "px",
    unit: "m_e c",
    processData: (data) => data.px,
  },
  py: {
    name: "py",
    unit: "m_e c",
    processData: (data) => data.py,
  },
  pz: {
    name: "pz",
    unit: "m_e c",
    processData: (data) => data.pz,
  },
  Ek: {
    name: "Ek",
    unit: "m_e c2",
    processData: (data) => data.Ek,
  },
  t: {
    name: "t",
    unit: "1/ω",
    processData: (data) => data.t,
  }
};

class Figure2d extends Figure {
  constructor(plot, id, axes) {
    super(plot, id, axes);
    this.x = Figure.variables[axes.x];
    this.y = Figure.variables[axes.y];
    this.plot.setOption({
      tooltip: {},
      backgroundColor: '#fff',
      xAxis: {
        type: 'value',
        name: `${this.x.name}${this.x.unit ? ` [${this.x.unit}]` : ''}`,
      },
      yAxis: {
        type: 'value',
        name: `${this.y.name}${this.y.unit ? ` [${this.y.unit}]` : ''}`,
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

  addData(data, isNewParticle) {
    if (isNewParticle) {
      this.series.push(this.getSeriesElement());
    }
    const x = this.x.processData(data);
    const y = this.y.processData(data);
    this.series[this.series.length - 1].data.push([x, y]);
  }
}

class Figure3d extends Figure {
  constructor(plot, id, axes) {
    super(plot, id, axes);
    this.x = Figure.variables[axes.x];
    this.y = Figure.variables[axes.y];
    this.z = Figure.variables[axes.z];
    this.plot.setOption({
      tooltip: {},
      backgroundColor: '#fff',
      xAxis3D: {
        type: 'value',
        name: `${this.x.name}${this.x.unit ? ` [${this.x.unit}]` : ''}`,
      },
      yAxis3D: {
        type: 'value',
        name: `${this.y.name}${this.y.unit ? ` [${this.y.unit}]` : ''}`,
      },
      zAxis3D: {
        type: 'value',
        name: `${this.z.name}${this.z.unit ? ` [${this.z.unit}]` : ''}`,
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

  addData(data, isNewParticle) {
    if (isNewParticle) {
      this.series.push(this.getSeriesElement());
    }
    const x = this.x.processData(data);
    const y = this.y.processData(data);
    const z = this.z.processData(data);
    this.series[this.series.length - 1].data.push([x, y, z]);
  }
}