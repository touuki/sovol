$(function () {

    custom_function = {};
    figures = {};
    sim_start = false;
    sim_id = 0;

    function checkReady() {
        if (!Module)
            return;
        $('#start').removeAttr("disabled");
    };
    setTimeout(checkReady, 1000);

    $('#start').click(function () {
        if (sim_start) {
            return;
        } else {
            clearFigures();
            sim_start = true;
            let key_perfix = 'SC_';
            let obj = {};
            $('.simulation-input').filter(':visible').each((i, e) => {
                if ($(e).hasClass('eval')) {
                    obj[key_perfix + e.name] = eval(e.value).toString();
                } else {
                    obj[key_perfix + e.name] = e.value.toString();
                }
            });
            if (obj[key_perfix + 'FIELD_CLASSNAME'] == 'CustomField') {
                let constants_string = $('#constants_table').children().map(function (i, e) {
                    let je = $(e);
                    return 'var ' + je.find('.customfield-constant-key').val()
                        + '=' + je.find('.customfield-constant-value').val();
                }).get().join(';');
                $('.customfield-func').each((i, e) => {
                    if (custom_function[e.name]) {
                        removeFunction(custom_function[e.name]);
                    }
                    custom_function[e.name] = addFunction(new Function('x', 'y', 'z', 't', constants_string + ';' + e.value), 'ddddd');
                    obj[key_perfix + e.name] = custom_function[e.name].toString();
                });
            }

            rateControl = {
                fps: $('#rateControl_fps').val(),
                dpf: $('#rateControl_dpf').val(),
            };
            nextFrame(Module.initSimulation(obj), ++sim_id, rateControl);
        }
    });

    function nextFrame(sim, id, rateControl) {
        let updatetime = new Date().getTime() + 1000 / rateControl.fps;
        for (let i = 0; i < rateControl.dpf && sim_start && id == sim_id;) {
            let data = Module.runAndGetData(sim);
            if (data.finished) {
                updateFrame();
                sim_start = false;
                delete sim;
                break;
            } else {
                if (data.data) {
                    if (data.isEndTime) {
                        addData(data.data, true);
                    } else {
                        addData(data.data);
                    }
                    i++;
                }
            }
        }
        if (sim_start && id == sim_id) {
            updateFrame();
            let nowtime = new Date().getTime();
            if (nowtime > updatetime) {
                setTimeout(nextFrame, 0, sim, id, rateControl);
            } else {
                setTimeout(nextFrame, updatetime - nowtime, sim, id, rateControl);
            }
        } else {
            delete sim;
        }

    };

    $('#stop').click(() => {
        sim_start = false;
    });

    function changeFieldType() {
        let name = $('#FIELD_CLASSNAME > option:selected').val();
        $('.field-args').css("display", "none");
        $('#' + name + '_args_div').css("display", "");
    }

    changeFieldType();
    $('#FIELD_CLASSNAME').change(changeFieldType);

    variables = {
        x: {
            unit: "c/ω",
            processData: (data) => data.x,
        },
        y: {
            unit: "c/ω",
            processData: (data) => data.y,
        },
        z: {
            unit: "c/ω",
            processData: (data) => data.z,
        },
        px: {
            unit: "m_e c",
            processData: (data) => data.px,
        },
        py: {
            unit: "m_e c",
            processData: (data) => data.py,
        },
        pz: {
            unit: "m_e c",
            processData: (data) => data.pz,
        },
        Ek: {
            unit: "m_e c2",
            processData: (data) => data.Ek,
        },
        t: {
            unit: "1/ω",
            processData: (data) => data.t,
        }
    }


    $('#particles_plots_input select').each((i, e) => {
        let je = $(e);
        for (const key in variables) {
            je.append('<option value="' + key + '">' + key + '</option>');
        }
    });

    $('#add_particles_plot').click(function () {
        let figure = {};
        let inp = {};
        $('#particles_plots_input').find('input, select').each((i, e) => inp[e.name] = e.value);
        let id = 'particles_plot_' + Math.random().toString().substr(-8);
        let title = inp.xaxis + '-' + inp.yaxis;
        if (inp.zaxis) {
            title += '-' + inp.zaxis;
        }
        $('#particles_plots').prepend(
            '<div class="input-div">\
                <div class="table-title">' + title + '</div>\
                <div class="figure" id="'+ id + '">\
                </div>\
                <table class="input-table">\
                    <tr>\
                        <td>\
                            <button class="remove-button red-button" data-figureid="'+ id + '">Remove</button>\
                        </td>\
                    </tr>\
                </table>\
            </div>');
        let plot = echarts.init($('#' + id)[0]);
        figure.plot = plot;
        figure.id = id;
        figure.initSeries = function () {
            this.series = [this.getSeriesElement()];
        };
        figure.updateFrame = function () {
            this.plot.setOption({ series: this.series })
        };
        figure.clear = function () {
            for (let i = 0; i < this.series.length; i++) {
                this.series[i] = this.getSeriesElement();
            }
            this.updateFrame();
            this.initSeries();
        };

        if (inp.zaxis) {
            plot.setOption({
                tooltip: {},
                backgroundColor: '#fff',
                xAxis3D: {
                    type: 'value',
                    name: inp.xaxis + ' [' + variables[inp.xaxis].unit + ']',
                },
                yAxis3D: {
                    type: 'value',
                    name: inp.yaxis + ' [' + variables[inp.yaxis].unit + ']',
                },
                zAxis3D: {
                    type: 'value',
                    name: inp.zaxis + ' [' + variables[inp.zaxis].unit + ']',
                },
                grid3D: {
                    viewControl: {
                        projection: 'orthographic'
                    }
                }
            });

            figure.x = variables[inp.xaxis];
            figure.y = variables[inp.yaxis];
            figure.z = variables[inp.zaxis];
            figure.getSeriesElement = function () {
                return {
                    type: 'line3D',
                    data: [],
                    lineStyle: {
                        width: 2
                    }
                };
            }
            figure.addData = function (data, isEndTime) {
                let x = this.x.processData(data);
                let y = this.y.processData(data);
                let z = this.z.processData(data);
                this.series[this.series.length - 1].data.push([x, y, z]);
                if (isEndTime) {
                    this.series.push(this.getSeriesElement());
                }
            };
        } else {
            plot.setOption({
                tooltip: {},
                backgroundColor: '#fff',
                xAxis: {
                    type: 'value',
                    name: inp.xaxis + ' [' + variables[inp.xaxis].unit + ']',
                },
                yAxis: {
                    type: 'value',
                    name: inp.yaxis + ' [' + variables[inp.yaxis].unit + ']',
                }
            });

            figure.x = variables[inp.xaxis];
            figure.y = variables[inp.yaxis];
            figure.getSeriesElement = function () {
                return {
                    type: 'line',
                    symbol: 'none',
                    data: [],
                    lineStyle: {
                        width: 2
                    }
                };
            };
            figure.addData = function (data, isEndTime) {
                let x = this.x.processData(data);
                let y = this.y.processData(data);
                this.series[this.series.length - 1].data.push([x, y]);
                if (isEndTime) {
                    this.series.push(this.getSeriesElement());
                }
            };

        }

        figure.initSeries();
        figures[id] = figure;
    });

    $('#particles_plots').on('click', '.remove-button', function () {
        let figure = figures[this.dataset.figureid];
        echarts.dispose(figure.plot);
        delete figures[this.dataset.figureid];
        $(this).parents('.input-div').remove();
    });

    $('#add_customfield_constant').click(function () {
        $('#constants_table').prepend(
            '<tr>\
                <td>\
                    <input class="customfield-constant-key" type="text">\
                </td>\
                <td style="padding: 0;">=</td>\
                <td>\
                    <input class="customfield-constant-value" type="text">\
                </td>\
                <td>\
                    <button class="remove-button red-button">Remove</button>\
                </td>\
            </tr>');
    });

    $('#constants_table').on('click', '.remove-button', function () {
        $(this).parents('tr').remove();
    });

    function addData(data, isEndTime) {
        for (const key in figures) {
            figures[key].addData(data, isEndTime);
        }
    };

    function updateFrame() {
        for (const key in figures) {
            figures[key].updateFrame();
        };
    };

    function clearFigures() {
        for (const key in figures) {
            figures[key].clear();
        }
    };

});