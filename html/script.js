const initSim = new ConditionalExecutor(() => {
  if (!Module || !Module.initTables) {
    return false;
  }
  for (const key in tables) {
    if (!tables[key].ready) {
      return false;
    }
  }
  return true;
}, () => {
  Module.initTables(tables);
  document.getElementById("start").disabled = '';
});
const version = "2.0";
const custom_function = {};
const figures = {};

const tables = {
  anomalousMagneticMoment: {
    url: "tables/anomalousMagneticMoment.data"
  },
  photonEmissionRate: {
    url: "tables/photonEmissionRate.data"
  },
  photonEmissionRateSpinCorrection: {
    url: "tables/photonEmissionRateSpinCorrection.data"
  },
  intK13: {
    url: "tables/intK13.data"
  },
  k13: {
    url: "tables/k13.data"
  },
  k23: {
    url: "tables/k23.data"
  },
  photonParameter: {
    url: "tables/photonParameter.data"
  },
  f1: {
    url: "tables/f1.data"
  },
  f2: {
    url: "tables/f2.data"
  },
  f3: {
    url: "tables/f3.data"
  },
};

for (const key in tables) {
  $.ajax({
    url: tables[key].url,
    dataType: "binary",
    ifModified: false,
    processData: false,
    responseType: 'arraybuffer',
    success: (data) => {
      tables[key].arraybuffer = data;
      tables[key].ready = true;
    },
    error: (xhr, status, err) => {
      console.error(xhr, status, err);
    },
  });
};

simulation_callback = (data) => Object.values(figures).forEach(e => e.addData(data))

$(function () {
  window.$grid = $('.masonry').masonry({
    columnWidth: 480,
    itemSelector: '.masonry-item',
    gutter: 10,
    fitWidth: true,
    transitionDuration: 0
  });

  $('#start').click(function () {
    if (Module.isSimulationStart()) {
      return;
    } else {
      Object.values(figures).forEach(e => e.clear());
      Module.startSimulation();
    }
  });

  $('#stop').click(() => Module.stopSimulation());

  $('#particles_plots_input select.axis').each((i, e) => {
    for (const key in Figure.variables) {
      $(e).append(`<option value="${key}">${Figure.variables[key].name}</option>`);
    }
  });

  function addParticlesPlot(options) {
    const id = `particles_plot_${Math.random().toString().substr(-8)}`;
    const element = $(
      `<div class="masonry-item light-background">
        <div class="figure" id="${id}"></div>
        <table class="input-table">
          <tr>
            <td>
              <button class="remove-figure button is-red is-fullwidth" data-figureid="${id}">
                <span class="icon"><i class="fas fa-minus"></i></span><span>Remove</span>
              </button>
            </td>
          </tr>
        </table>
      </div>`);
    for (const key in options) {
      element.find('.figure')[0].dataset[key] = options[key];
    }
    $grid.append(element).masonry('appended', element);
    const plot = echarts.init($(`#${id}`)[0]);

    const figureClass = eval(options.figureClass)
    if (Figure.isPrototypeOf(figureClass)) {
      figures[id] = new figureClass(plot, id, options)
    }
  }

  $('#add_particles_plot').click(function () {
    const options = {};
    $('#particles_plots_input').find('input, select').each((i, e) => options[e.id] = e.value);
    addParticlesPlot(options);
  });

  $('body').on('click', '.remove-figure', function () {
    const figure = figures[this.dataset.figureid];
    echarts.dispose(figure.plot);
    delete figures[this.dataset.figureid];
    $grid.masonry('remove', $(this).parents('.light-background'));
    $grid.masonry();
  });

  $('#add_customfield_argument').click(function () {
    $('#arguments_table').prepend(
      `<tr>
        <td><input class="customfield-argument-key" type="text"></td>
        <td style="padding: 0;">=</td>
        <td><input class="customfield-argument-value" type="text"></td>
        <td>
          <button class="remove-button button is-red is-fullwidth">
            <span class="icon"><i class="fas fa-minus"></i></span><span>Remove</span>
          </button>
        </td>
      </tr>`);
    $grid.masonry();
  });

  $('#arguments_table').on('click', '.remove-button', function () {
    $(this).parents('tr').first().remove();
    $grid.masonry();
  });

  $('#export_data').click(function () {
    funDownload(JSON.stringify({ version: version, data: sim.getStoredData() }), 'sovol_data.json');
  });

  $('#import_args').click(() => $('#import_args_file').click());

  $('#import_args_file').on('change', function () {
    if (this.value === '') {
      return;
    }
    const reader = new FileReader();
    reader.onload = function (e) {
      const obj = JSON.parse(e.target.result);
      if (obj.static_inputs instanceof Array) {
        for (let i = 0; i < obj.static_inputs.length; i++) {
          const element = obj.static_inputs[i];
          if ($(`#${element.key}`)) {
            $(`#${element.key}`).val(element.value);
          }
        }
      }
      $('#FIELD_CLASSNAME').change();
      $('#arguments_table').find('.remove-button').click();
      if (obj.customfield_arguments instanceof Array) {
        for (let i = obj.customfield_arguments.length - 1; i >= 0; i--) {
          $('#add_customfield_argument').click();
          const element = obj.customfield_arguments[i];
          const je = $('#arguments_table').children(':first');
          je.find('.customfield-argument-key').val(element.key);
          je.find('.customfield-argument-value').val(element.value);
        }
      }
      $('.remove-figure').click();
      if (obj.plot_figures instanceof Array) {
        for (const options of obj.plot_figures) {
          addParticlesPlot(options)
        }
      }
    };
    reader.readAsText(this.files[0]);
    this.value = '';
  });

});