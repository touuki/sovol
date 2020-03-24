$(function () {
    $grid = $('.masonry').masonry({
        columnWidth: 480,
        itemSelector: '.masonry-item',
        gutter: 10,
        fitWidth: true,
        transitionDuration: 0
    });
    version = "1.0"
    custom_function = {};
    figures = {};
    sim_start = false;
    sim_id = 0;
    sim_data = null;
    function getDataObject() {
        return {
            x: [],
            y: [],
            z: [],
            px: [],
            py: [],
            pz: [],
            t: []
        };
    }

    function checkReady() {
        if (!Module)
            return;
        $('#start').removeAttr("disabled");
    };
    setTimeout(checkReady, 1000);

    function funDownload(content, filename) {
        var eleLink = document.createElement('a');
        eleLink.download = filename;
        eleLink.style.display = 'none';
        var blob = new Blob([content]);
        eleLink.href = URL.createObjectURL(blob);
        document.body.appendChild(eleLink);
        eleLink.click();
        document.body.removeChild(eleLink);
    };

    $('#start').click(function () {
        if (sim_start) {
            return;
        } else {
            clearSimulation();
            sim_start = true;
            let key_perfix = 'SC_';
            let obj = {};
            $('.simulation-input').filter(':visible').each((i, e) => {
                if ($(e).hasClass('eval')) {
                    obj[key_perfix + e.id] = eval(e.value).toString();
                } else {
                    obj[key_perfix + e.id] = e.value.toString();
                }
            });
            if (obj[key_perfix + 'FIELD_CLASSNAME'] == 'CustomField') {
                let arguments_string = $('#arguments_table').children().map(function (i, e) {
                    let je = $(e);
                    return 'var ' + je.find('.customfield-argument-key').val()
                        + '=' + je.find('.customfield-argument-value').val();
                }).get().join(';');
                $('.customfield-func').each((i, e) => {
                    if (custom_function[e.id]) {
                        removeFunction(custom_function[e.id]);
                    }
                    custom_function[e.id] = addFunction(new Function('x', 'y', 'z', 't', arguments_string + ';' + e.value), 'ddddd');
                    obj[key_perfix + e.id] = custom_function[e.id].toString();
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
                sim_data.pop();
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

    $('#FIELD_CLASSNAME').change(function () {
        let name = $('#FIELD_CLASSNAME > option:selected').val();
        $('.field-args').css("display", "none");
        $('#' + name + '_args_div').css("display", "");
        $grid.masonry();
    });

    $('#FIELD_CLASSNAME').change();

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

    function addParticlesPlot(xAxis, yAxis, zAxis) {
        let figure = {};
        let id = 'particles_plot_' + Math.random().toString().substr(-8);
        let title = xAxis + '-' + yAxis;
        if (zAxis) {
            title += '-' + zAxis;
        }
        let element = $(
            '<div class="masonry-item light-background">\
                <div class="table-title">' + title + '</div>\
                <div class="figure" id="'+ id + '" data-x-axis="' + xAxis + '" data-y-axis="' + yAxis + '" data-z-axis="' + zAxis + '">\
                </div>\
                <table class="input-table">\
                    <tr>\
                        <td>\
                            <button class="remove-figure button is-red is-fullwidth" data-figureid="'+ id + '">\
                                <span class="icon"><i class="fas fa-minus"></i></span><span>Remove</span>\
                            </button>\
                        </td>\
                    </tr>\
                </table>\
            </div>');
        $grid.append(element).masonry('appended', element);
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

        if (zAxis) {
            plot.setOption({
                tooltip: {},
                backgroundColor: '#fff',
                xAxis3D: {
                    type: 'value',
                    name: xAxis + ' [' + variables[xAxis].unit + ']',
                },
                yAxis3D: {
                    type: 'value',
                    name: yAxis + ' [' + variables[yAxis].unit + ']',
                },
                zAxis3D: {
                    type: 'value',
                    name: zAxis + ' [' + variables[zAxis].unit + ']',
                },
                grid3D: {
                    viewControl: {
                        projection: 'orthographic'
                    }
                }
            });

            figure.x = variables[xAxis];
            figure.y = variables[yAxis];
            figure.z = variables[zAxis];
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
                    name: xAxis + ' [' + variables[xAxis].unit + ']',
                },
                yAxis: {
                    type: 'value',
                    name: yAxis + ' [' + variables[yAxis].unit + ']',
                }
            });

            figure.x = variables[xAxis];
            figure.y = variables[yAxis];
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
    };

    $('#add_particles_plot').click(function () {
        let inp = {};
        $('#particles_plots_input').find('input, select').each((i, e) => inp[e.id] = e.value);
        addParticlesPlot(inp.xAxis, inp.yAxis, inp.zAxis);
    });

    $('body').on('click', '.remove-figure', function () {
        let figure = figures[this.dataset.figureid];
        echarts.dispose(figure.plot);
        delete figures[this.dataset.figureid];
        $grid.masonry('remove', $(this).parents('.light-background'));
        $grid.masonry();
    });

    $('#add_customfield_argument').click(function () {
        $('#arguments_table').prepend(
            '<tr>\
                <td>\
                    <input class="customfield-argument-key" type="text">\
                </td>\
                <td style="padding: 0;">=</td>\
                <td>\
                    <input class="customfield-argument-value" type="text">\
                </td>\
                <td>\
                    <button class="remove-button button is-red is-fullwidth">\
                        <span class="icon"><i class="fas fa-minus"></i></span><span>Remove</span>\
                    </button>\
                </td>\
            </tr>');
        $grid.masonry();
    });

    $('#arguments_table').on('click', '.remove-button', function () {
        $(this).parents('tr').remove();
        $grid.masonry();
    });

    function addData(data, isEndTime) {
        let obj = sim_data[sim_data.length - 1];
        obj.x.push(data.x);
        obj.y.push(data.y);
        obj.z.push(data.z);
        obj.px.push(data.px);
        obj.py.push(data.py);
        obj.pz.push(data.pz);
        obj.t.push(data.t);
        if (isEndTime) {
            sim_data.push(getDataObject());
        }
        for (const key in figures) {
            figures[key].addData(data, isEndTime);
        }
    };

    function updateFrame() {
        for (const key in figures) {
            figures[key].updateFrame();
        };
    };

    function clearSimulation() {
        sim_data = [getDataObject()];
        for (const key in figures) {
            figures[key].clear();
        }
    };

    $('#export_data').click(function () {
        funDownload(JSON.stringify({ version: version, data: sim_data }), 'sovol_data.json');
    });

    $('#export_args').click(function () {
        var obj = {};
        obj.version = version;
        obj.static_inputs = $('.static-input').map(function (i, e) {
            return {
                key: e.id,
                value: e.value
            };
        }).get();
        obj.customfield_arguments = $('#arguments_table').children().map(function (i, e) {
            let je = $(e);
            return {
                key: je.find('.customfield-argument-key').val(),
                value: je.find('.customfield-argument-value').val()
            };
        }).get();
        obj.plot_figures = $('.figure').map(function (i, e) {
            return {
                xAxis: e.dataset.xAxis,
                yAxis: e.dataset.yAxis,
                zAxis: e.dataset.zAxis
            };
        }).get();
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
                    if ($('#' + element.key)) {
                        $('#' + element.key).val(element.value);
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
                for (let i = obj.plot_figures.length - 1; i >= 0; i--) {
                    const element = obj.plot_figures[i];
                    addParticlesPlot(element.xAxis, element.yAxis, element.zAxis);
                }
            }
        };
        reader.readAsText(this.files[0]);
    });

});