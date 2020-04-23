$.ajaxTransport("+binary", function (options, originalOptions, jqXHR) {
  // check for conditions and support for blob / arraybuffer response type
  if (window.FormData && ((options.dataType && (options.dataType == 'binary'))
    || (options.data && ((window.ArrayBuffer && options.data instanceof ArrayBuffer)
      || (window.Blob && options.data instanceof Blob))))) {
    return {
      // create new XMLHttpRequest
      send: function (headers, callback) {
        // setup all variables
        var xhr = new XMLHttpRequest(),
          url = options.url,
          type = options.type,
          async = options.async || true,
          // blob or arraybuffer. Default is blob
          dataType = options.responseType || "blob",
          data = options.data || null,
          username = options.username || null,
          password = options.password || null;

        xhr.addEventListener('load', function () {
          var data = {};
          data[options.dataType] = xhr.response;
          // make callback and send data
          callback(xhr.status, xhr.statusText, data, xhr.getAllResponseHeaders());
        });

        xhr.open(type, url, async, username, password);

        // setup custom headers
        for (var i in headers) {
          xhr.setRequestHeader(i, headers[i]);
        }

        xhr.responseType = dataType;
        xhr.send(data);
      },
      abort: function () {
        jqXHR.abort();
      }
    };
  }
});

function funDownload(content, filename) {
  var eleLink = document.createElement('a');
  eleLink.download = filename;
  eleLink.style.display = 'none';
  var blob = new Blob([content]);
  eleLink.href = URL.createObjectURL(blob);
  document.body.appendChild(eleLink);
  eleLink.click();
  document.body.removeChild(eleLink);
}

Object.values = Object.values || (obj => Object.keys(obj).map(key => obj[key]));

class ConditionalExecutor {
  constructor(conditional, execution, options) {
    this.conditional = conditional;
    this.execution = execution;
    options = options || {};
    this.interval = options.interval || 300;
    this.started = false;
    this.finished = false;
  }

  get conditional() {
    return this._conditional;
  }

  set conditional(value) {
    if (typeof value !== 'function') {
      throw new Error("Conditional must be a function.");
    }
    this._conditional = value;
  }

  get execution() {
    return this._execution;
  }

  set execution(value) {
    if (typeof value !== 'function') {
      throw new Error("Callback must be a function.");
    }
    this._execution = value;
  }

  check() {
    if (this.started) {
      if (this.conditional()) {
        this.finished = true;
        this.execution();
      } else {
        setTimeout((self) => self.check(), this.interval, this);
      }
    }
  }

  stop() {
    if (this.finished) {
      throw new Error("The Executor has already finished.");
    }
    this.started = false;
  }

  start() {
    if (this.finished) {
      throw new Error("The Executor has already finished.");
    }
    if (this.started) {
      throw new Error("The Executor has already started.");;
    }
    this.started = true;
    this.check();
  }
}