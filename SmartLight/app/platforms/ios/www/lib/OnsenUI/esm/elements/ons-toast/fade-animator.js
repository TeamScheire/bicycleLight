import _Object$getPrototypeOf from 'babel-runtime/core-js/object/get-prototype-of';
import _classCallCheck from 'babel-runtime/helpers/classCallCheck';
import _createClass from 'babel-runtime/helpers/createClass';
import _possibleConstructorReturn from 'babel-runtime/helpers/possibleConstructorReturn';
import _inherits from 'babel-runtime/helpers/inherits';

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

import animit from '../../ons/animit';
import ToastAnimator from './animator';

/**
 * iOS style animator for dialog.
 */

var FadeToastAnimator = function (_ToastAnimator) {
  _inherits(FadeToastAnimator, _ToastAnimator);

  function FadeToastAnimator() {
    var _ref = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : {},
        _ref$timing = _ref.timing,
        timing = _ref$timing === undefined ? 'linear' : _ref$timing,
        _ref$delay = _ref.delay,
        delay = _ref$delay === undefined ? 0 : _ref$delay,
        _ref$duration = _ref.duration,
        duration = _ref$duration === undefined ? 0.3 : _ref$duration;

    _classCallCheck(this, FadeToastAnimator);

    return _possibleConstructorReturn(this, (FadeToastAnimator.__proto__ || _Object$getPrototypeOf(FadeToastAnimator)).call(this, { timing: timing, delay: delay, duration: duration }));
  }

  /**
   * @param {HTMLElement} toast
   * @param {Function} callback
   */


  _createClass(FadeToastAnimator, [{
    key: 'show',
    value: function show(toast, callback) {
      callback = callback ? callback : function () {};

      animit(toast, this.def).default({ opacity: 0 }, { opacity: 1 }).queue(function (done) {
        callback();
        done();
      }).play();
    }

    /**
     * @param {HTMLElement} toast
     * @param {Function} callback
     */

  }, {
    key: 'hide',
    value: function hide(toast, callback) {
      callback = callback ? callback : function () {};

      animit(toast, this.def).default({ opacity: 1 }, { opacity: 0 }).queue(function (done) {
        callback();
        done();
      }).play();
    }
  }]);

  return FadeToastAnimator;
}(ToastAnimator);

export default FadeToastAnimator;