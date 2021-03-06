const initSim = new ConditionalExecutor(() => Module && Module.getWasmSimulation, () => {
  window.sim = Module.getWasmSimulation("WasmSimulation");
  document.getElementById("start").disabled = '';
});
const version = "1.2.0";
const custom_function = {};
const figures = {};

$(function () {
  window.$grid = $('.masonry').masonry({
    columnWidth: 480,
    itemSelector: '.masonry-item',
    gutter: 10,
    fitWidth: true,
    transitionDuration: 0
  });

  $('#start').click(function () {
    if (!sim || sim.isStart()) {
      return;
    } else {
      Object.values(figures).forEach(e => e.clear());
      const perfix = 'SC_';
      const obj = {};
      $('.simulation-input').filter(':visible').each((i, e) =>
        obj[`${perfix}${e.id}`] = $(e).hasClass('eval') ? eval(e.value).toString() : e.value.toString()
      );
      if (obj[`${perfix}FIELD_CLASSNAME`] == 'CustomField') {
        const arguments_string = $('#arguments_table').children().map(function (i, e) {
          return `var ${$(e).find('.customfield-argument-key').val()} = ${$(e).find('.customfield-argument-value').val()};`;
        }).get().join('');
        $('.customfield-func').each((i, e) => {
          if (custom_function[e.id]) {
            removeFunction(custom_function[e.id]);
          }
          custom_function[e.id] = addFunction(new Function('x', 'y', 'z', 't', `${arguments_string}${e.value}`), 'ddddd');
          obj[`${perfix}${e.id}`] = custom_function[e.id].toString();
        });
      }

      const rateControl = {
        fps: $('#rateControl_fps').val(),
        dpf: $('#rateControl_dpf').val(),
      };
      sim.init(obj);
      nextFrame(sim.getId(), rateControl);
    }
  });

  function nextFrame(id, rateControl) {
    const updatetime = new Date().getTime() + 1000 / rateControl.fps;
    for (let i = 0; i < rateControl.dpf;) {
      const data = sim.runAndGetData(id);
      if (data.aborted) {
        return;
      }
      if (data.finished) {
        Object.values(figures).forEach(e => e.updateFrame());
        return;
      }
      if (data.data) {
        Object.values(figures).forEach(e => e.addData(data))
        i++
      }
    }
    Object.values(figures).forEach(e => e.updateFrame());
    const nowtime = new Date().getTime();
    if (nowtime > updatetime) {
      setTimeout(nextFrame, 0, id, rateControl);
    } else {
      setTimeout(nextFrame, updatetime - nowtime, id, rateControl);
    }
  }

  $('#stop').click(() => {
    sim && sim.stop();
  });

  $('#FIELD_CLASSNAME').change(function () {
    const name = $('#FIELD_CLASSNAME > option:selected').val();
    $('.field-args').css("display", "none");
    $(`#${name}_args_div`).css("display", "");
    $grid.masonry();
  });

  $('#FIELD_CLASSNAME').change();

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

  $('#export_args').click(function () {
    const obj = {};
    obj.version = version;
    obj.static_inputs = $('.static-input').map(function (i, e) {
      return {
        key: e.id,
        value: e.value
      };
    }).get();
    obj.customfield_arguments = $('#arguments_table').children().map(function (i, e) {
      return {
        key: $(e).find('.customfield-argument-key').val(),
        value: $(e).find('.customfield-argument-value').val()
      };
    }).get();
    obj.plot_figures = $('.figure').map((i, e) => e.dataset).get();
    funDownload(JSON.stringify(obj), 'sovol_args.json');
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