/*
Copyright 2013-2015 ASIAL CORPORATION

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

*/

import platform from './platform';
import util from './util';

var error = function error(message) {
  return util.throw('In PageAttributeExpression: ' + message);
};

var pageAttributeExpression = {
  _variables: {},

  /**
   * Define a variable.
   *
   * @param {String} name Name of the variable
   * @param {String|Function} value Value of the variable. Can be a string or a function. The function must return a string.
   * @param {Boolean} overwrite If this value is false, an error will be thrown when trying to define a variable that has already been defined.
   */
  defineVariable: function defineVariable(name, value) {
    var overwrite = arguments.length > 2 && arguments[2] !== undefined ? arguments[2] : false;

    if (typeof name !== 'string') {
      error('Variable name must be a string');
    } else if (typeof value !== 'string' && typeof value !== 'function') {
      error('Variable value must be a string or a function');
    } else if (this._variables.hasOwnProperty(name) && !overwrite) {
      error('"' + name + '" is already defined');
    }
    this._variables[name] = value;
  },

  /**
   * Get a variable.
   *
   * @param {String} name Name of the variable.
   * @return {String|Function|null}
   */
  getVariable: function getVariable(name) {
    if (!this._variables.hasOwnProperty(name)) {
      return null;
    }

    return this._variables[name];
  },

  /**
   * Remove a variable.
   *
   * @param {String} name Name of the varaible.
   */
  removeVariable: function removeVariable(name) {
    delete this._variables[name];
  },

  /**
   * Get all variables.
   *
   * @return {Object}
   */
  getAllVariables: function getAllVariables() {
    return this._variables;
  },
  _parsePart: function _parsePart(part) {
    var c = void 0,
        inInterpolation = false,
        currentIndex = 0;

    var tokens = [];

    if (part.length === 0) {
      error('Unable to parse empty string');
    }

    for (var i = 0; i < part.length; i++) {
      c = part.charAt(i);

      if (c === '$' && part.charAt(i + 1) === '{') {
        if (inInterpolation) {
          error('Nested interpolation not supported');
        }

        var token = part.substring(currentIndex, i);
        if (token.length > 0) {
          tokens.push(part.substring(currentIndex, i));
        }

        currentIndex = i;
        inInterpolation = true;
      } else if (c === '}') {
        if (!inInterpolation) {
          error('} must be preceeded by ${');
        }

        var _token = part.substring(currentIndex, i + 1);
        if (_token.length > 0) {
          tokens.push(part.substring(currentIndex, i + 1));
        }

        currentIndex = i + 1;
        inInterpolation = false;
      }
    }

    if (inInterpolation) {
      error('Unterminated interpolation');
    }

    tokens.push(part.substring(currentIndex, part.length));

    return tokens;
  },
  _replaceToken: function _replaceToken(token) {
    var re = /^\${(.*?)}$/,
        match = token.match(re);

    if (match) {
      var name = match[1].trim();
      var variable = this.getVariable(name);

      if (variable === null) {
        error('Variable "' + name + '" does not exist');
      } else if (typeof variable === 'string') {
        return variable;
      } else {
        var rv = variable();

        if (typeof rv !== 'string') {
          error('Must return a string');
        }

        return rv;
      }
    } else {
      return token;
    }
  },
  _replaceTokens: function _replaceTokens(tokens) {
    return tokens.map(this._replaceToken.bind(this));
  },
  _parseExpression: function _parseExpression(expression) {
    return expression.split(',').map(function (part) {
      return part.trim();
    }).map(this._parsePart.bind(this)).map(this._replaceTokens.bind(this)).map(function (part) {
      return part.join('');
    });
  },

  /**
   * Evaluate an expression.
   *
   * @param {String} expression An page attribute expression.
   * @return {Array}
   */
  evaluate: function evaluate(expression) {
    if (!expression) {
      return [];
    }

    return this._parseExpression(expression);
  }
};

// Define default variables.
pageAttributeExpression.defineVariable('mobileOS', platform.getMobileOS());
pageAttributeExpression.defineVariable('iOSDevice', platform.getIOSDevice());
pageAttributeExpression.defineVariable('runtime', function () {
  return platform.isWebView() ? 'cordova' : 'browser';
});

export default pageAttributeExpression;