import _Object$getPrototypeOf from 'babel-runtime/core-js/object/get-prototype-of';
import _classCallCheck from 'babel-runtime/helpers/classCallCheck';
import _createClass from 'babel-runtime/helpers/createClass';
import _possibleConstructorReturn from 'babel-runtime/helpers/possibleConstructorReturn';
import _get from 'babel-runtime/helpers/get';
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

import NavigatorAnimator from './animator';
import util from '../../ons/util';
import animit from '../../ons/animit';

var transform = 'translate3d(0, 0, 0)';

/**
 * Fade-in screen transition.
 */

var IOSFadeNavigatorAnimator = function (_NavigatorAnimator) {
  _inherits(IOSFadeNavigatorAnimator, _NavigatorAnimator);

  function IOSFadeNavigatorAnimator() {
    var _ref = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : {},
        _ref$timing = _ref.timing,
        timing = _ref$timing === undefined ? 'linear' : _ref$timing,
        _ref$delay = _ref.delay,
        delay = _ref$delay === undefined ? 0 : _ref$delay,
        _ref$duration = _ref.duration,
        duration = _ref$duration === undefined ? 0.4 : _ref$duration;

    _classCallCheck(this, IOSFadeNavigatorAnimator);

    return _possibleConstructorReturn(this, (IOSFadeNavigatorAnimator.__proto__ || _Object$getPrototypeOf(IOSFadeNavigatorAnimator)).call(this, { timing: timing, delay: delay, duration: duration }));
  }

  /**
   * @param {Object} enterPage
   * @param {Object} leavePage
   * @param {Function} callback
   */


  _createClass(IOSFadeNavigatorAnimator, [{
    key: 'push',
    value: function push(enterPage, leavePage, callback) {
      var unblock = _get(IOSFadeNavigatorAnimator.prototype.__proto__ || _Object$getPrototypeOf(IOSFadeNavigatorAnimator.prototype), 'block', this).call(this, enterPage);

      animit.runAll(animit(enterPage, this.def).default({ transform: transform, opacity: 0 }, { transform: transform, opacity: 1 }).queue(function (done) {
        unblock();
        callback();
        done();
      }));
    }

    /**
     * @param {Object} enterPage
     * @param {Object} leavePage
     * @param {Function} done
     */

  }, {
    key: 'pop',
    value: function pop(enterPage, leavePage, callback) {
      var unblock = _get(IOSFadeNavigatorAnimator.prototype.__proto__ || _Object$getPrototypeOf(IOSFadeNavigatorAnimator.prototype), 'block', this).call(this, enterPage);

      animit.runAll(animit(leavePage, this.def).default({ transform: transform, opacity: 1 }, { transform: transform, opacity: 0 }).queue(function (done) {
        unblock();
        callback();
        done();
      }));
    }
  }]);

  return IOSFadeNavigatorAnimator;
}(NavigatorAnimator);

export default IOSFadeNavigatorAnimator;