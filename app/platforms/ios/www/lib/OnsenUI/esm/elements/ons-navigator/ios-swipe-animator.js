import _Object$keys from 'babel-runtime/core-js/object/keys';
import _WeakMap from 'babel-runtime/core-js/weak-map';
import _extends from 'babel-runtime/helpers/extends';
import _Object$getPrototypeOf from 'babel-runtime/core-js/object/get-prototype-of';
import _objectWithoutProperties from 'babel-runtime/helpers/objectWithoutProperties';
import _classCallCheck from 'babel-runtime/helpers/classCallCheck';
import _possibleConstructorReturn from 'babel-runtime/helpers/possibleConstructorReturn';
import _get from 'babel-runtime/helpers/get';
import _createClass from 'babel-runtime/helpers/createClass';
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

/**
 * Abstract swipe animator for iOS navigator transition.
 */

var IOSSwipeNavigatorAnimator = function (_NavigatorAnimator) {
  _inherits(IOSSwipeNavigatorAnimator, _NavigatorAnimator);

  _createClass(IOSSwipeNavigatorAnimator, null, [{
    key: 'swipeable',
    get: function get() {
      return true;
    }
  }]);

  function IOSSwipeNavigatorAnimator() {
    var _ref = arguments.length > 0 && arguments[0] !== undefined ? arguments[0] : {};

    var _ref$durationRestore = _ref.durationRestore,
        durationRestore = _ref$durationRestore === undefined ? 0.1 : _ref$durationRestore,
        _ref$durationSwipe = _ref.durationSwipe,
        durationSwipe = _ref$durationSwipe === undefined ? 0.15 : _ref$durationSwipe,
        _ref$timingSwipe = _ref.timingSwipe,
        timingSwipe = _ref$timingSwipe === undefined ? 'linear' : _ref$timingSwipe,
        rest = _objectWithoutProperties(_ref, ['durationRestore', 'durationSwipe', 'timingSwipe']);

    _classCallCheck(this, IOSSwipeNavigatorAnimator);

    var _this = _possibleConstructorReturn(this, (IOSSwipeNavigatorAnimator.__proto__ || _Object$getPrototypeOf(IOSSwipeNavigatorAnimator)).call(this, _extends({}, rest)));

    if (_this.constructor === IOSSwipeNavigatorAnimator) {
      util.throwAbstract();
    }

    _this.durationRestore = durationRestore;
    _this.durationSwipe = durationSwipe;
    _this.timingSwipe = timingSwipe;

    _this.optSwipe = { timing: timingSwipe, duration: durationSwipe };
    _this.optRestore = { timing: timingSwipe, duration: durationRestore };

    _this.swipeShadow = util.createElement('<div style="position: absolute; height: 100%; width: 12px; right: 100%; top: 0; bottom: 0; z-index: -1;' + 'background: linear-gradient(to right, transparent 0, rgba(0,0,0,.04) 40%, rgba(0,0,0,.12) 80%, rgba(0,0,0,.16) 100%);"></div>');

    _this.isDragStart = true;
    return _this;
  }

  _createClass(IOSSwipeNavigatorAnimator, [{
    key: '_decompose',
    value: function _decompose() {
      util.throwMember();
    }
  }, {
    key: '_shouldAnimateToolbar',
    value: function _shouldAnimateToolbar() {
      util.throwMember();
    }
  }, {
    key: '_calculateDelta',
    value: function _calculateDelta() {
      util.throwMember();
    }
  }, {
    key: '_dragStartSetup',
    value: function _dragStartSetup(enterPage, leavePage) {
      this.isDragStart = false;

      // Avoid content clicks
      this.unblock = _get(IOSSwipeNavigatorAnimator.prototype.__proto__ || _Object$getPrototypeOf(IOSSwipeNavigatorAnimator.prototype), 'block', this).call(this, leavePage);

      // Mask
      enterPage.parentElement.insertBefore(this.backgroundMask, enterPage);

      // Decomposition
      this.target = {
        enter: util.findToolbarPage(enterPage) || enterPage,
        leave: util.findToolbarPage(leavePage) || leavePage
      };
      this.decomp = {
        enter: this._decompose(this.target.enter),
        leave: this._decompose(this.target.leave)
      };

      // Animation values
      this.delta = this._calculateDelta(leavePage, this.decomp.leave);
      this.shouldAnimateToolbar = this._shouldAnimateToolbar(this.target.enter, this.target.leave);

      // Shadow && styles
      if (this.shouldAnimateToolbar) {
        this.swipeShadow.style.top = this.decomp.leave.toolbar.offsetHeight + 'px';
        this.target.leave.appendChild(this.swipeShadow);
        this._saveStyle(this.target.enter, this.target.leave);
      } else {
        leavePage.appendChild(this.swipeShadow);
        this._saveStyle(enterPage, leavePage);
      }
      leavePage.classList.add('overflow-visible');
      this.overflowElement = leavePage;
      this.decomp.leave.content.classList.add('content-swiping');
    }
  }, {
    key: 'translate',
    value: function translate(distance, maxWidth, enterPage, leavePage) {
      this.isSwiping = true;

      if (enterPage.style.display === 'none') {
        enterPage.style.display = '';
      }

      if (this.isDragStart) {
        this.maxWidth = maxWidth;
        this._dragStartSetup(enterPage, leavePage);
      }

      var swipeRatio = (distance - maxWidth) / maxWidth;

      if (this.shouldAnimateToolbar) {

        animit.runAll(

        /* Enter page */

        animit([this.decomp.enter.content, this.decomp.enter.bottomToolbar, this.decomp.enter.background]).queue({
          transform: 'translate3d(' + swipeRatio * 25 + '%, 0, 0)',
          opacity: 1 + swipeRatio * 10 / 100 // 0.9 -> 1
        }), animit(this.decomp.enter.toolbarCenter).queue({
          transform: 'translate3d(' + this.delta.title * swipeRatio + 'px, 0, 0)',
          opacity: 1 + swipeRatio // 0 -> 1
        }), animit(this.decomp.enter.backButtonLabel).queue({
          opacity: 1 + swipeRatio * 10 / 100, // 0.9 -> 1
          transform: 'translate3d(' + this.delta.label * swipeRatio + 'px, 0, 0)'
        }), animit(this.decomp.enter.other).queue({
          opacity: 1 + swipeRatio // 0 -> 1
        }),

        /* Leave page */

        animit([this.decomp.leave.content, this.decomp.leave.bottomToolbar, this.decomp.leave.background, this.swipeShadow]).queue({
          transform: 'translate3d(' + distance + 'px, 0, 0)'
        }), animit(this.decomp.leave.toolbar).queue({
          opacity: -1 * swipeRatio // 1 -> 0
        }), animit(this.decomp.leave.toolbarCenter).queue({
          transform: 'translate3d(' + (1 + swipeRatio) * 125 + '%, 0, 0)'
        }), animit(this.decomp.leave.backButtonLabel).queue({
          opacity: -1 * swipeRatio, // 1 -> 0
          transform: 'translate3d(' + this.delta.title * (1 + swipeRatio) + 'px, 0, 0)'
        }),

        /* Other */

        animit(this.swipeShadow).queue({
          opacity: -1 * swipeRatio // 1 -> 0
        }));
      } else {
        animit.runAll(animit(leavePage).queue({
          transform: 'translate3d(' + distance + 'px, 0, 0)'
        }), animit(enterPage).queue({
          transform: 'translate3d(' + swipeRatio * 25 + '%, 0, 0)',
          opacity: 1 + swipeRatio * 10 / 100 // 0.9 -> 1
        }), animit(this.swipeShadow).queue({
          opacity: -1 * swipeRatio // 1 -> 0
        }));
      }
    }
  }, {
    key: 'restore',
    value: function restore(enterPage, leavePage, callback) {
      var _this2 = this;

      if (this.isDragStart) {
        return;
      }

      if (this.shouldAnimateToolbar) {

        animit.runAll(

        /* Enter page */

        animit([this.decomp.enter.content, this.decomp.enter.bottomToolbar, this.decomp.enter.background]).queue({
          transform: 'translate3d(-25%, 0, 0)',
          opacity: 0.9
        }, this.optRestore), animit(this.decomp.enter.toolbarCenter).queue({
          transform: 'translate3d(-' + this.delta.title + 'px, 0, 0)',
          transition: 'opacity ' + this.durationRestore + 's linear, transform ' + this.durationRestore + 's ' + this.timingSwipe,
          opacity: 0
        }), animit(this.decomp.enter.backButtonLabel).queue({
          transform: 'translate3d(-' + this.delta.label + 'px, 0, 0)'
        }, this.optRestore), animit(this.decomp.enter.other).queue({
          opacity: 0
        }, this.optRestore),

        /* Leave page */

        animit([this.decomp.leave.content, this.decomp.leave.bottomToolbar, this.decomp.leave.background, this.swipeShadow]).queue({
          transform: 'translate3d(0, 0, 0)'
        }, this.optRestore), animit(this.decomp.leave.toolbar).queue({
          opacity: 1
        }, this.optRestore), animit(this.decomp.leave.toolbarCenter).queue({
          transform: 'translate3d(0, 0, 0)'
        }, this.optRestore), animit(this.decomp.leave.backButtonLabel).queue({
          opacity: 1,
          transform: 'translate3d(0, 0, 0)',
          transition: 'opacity ' + this.durationRestore + 's linear, transform ' + this.durationRestore + 's ' + this.timingSwipe
        }),

        /* Other */

        animit(this.swipeShadow).queue({
          opacity: 0
        }, this.optRestore).queue(function (done) {
          _this2._reset(_this2.target.enter, _this2.target.leave);
          enterPage.style.display = 'none';
          callback && callback();
          done();
        }));
      } else {
        animit.runAll(animit(enterPage).queue({
          transform: 'translate3D(-25%, 0, 0)',
          opacity: 0.9
        }, this.optRestore), animit(leavePage).queue({
          transform: 'translate3D(0, 0, 0)'
        }, this.optRestore).queue(function (done) {
          _this2._reset(enterPage, leavePage);
          enterPage.style.display = 'none';
          callback && callback();
          done();
        }));
      }
    }
  }, {
    key: 'popSwipe',
    value: function popSwipe(enterPage, leavePage, callback) {
      var _this3 = this;

      if (this.isDragStart) {
        return;
      }

      if (this.shouldAnimateToolbar) {

        animit.runAll(

        /* Enter page */

        animit([this.decomp.enter.content, this.decomp.enter.bottomToolbar, this.decomp.enter.background]).queue({
          transform: 'translate3d(0, 0, 0)',
          opacity: 1
        }, this.optSwipe), animit(this.decomp.enter.toolbarCenter).queue({
          transform: 'translate3d(0, 0, 0)',
          transition: 'opacity ' + this.durationSwipe + 's linear, transform ' + this.durationSwipe + 's ' + this.timingSwipe,
          opacity: 1
        }), animit(this.decomp.enter.backButtonLabel).queue({
          transform: 'translate3d(0, 0, 0)'
        }, this.optSwipe), animit(this.decomp.enter.other).queue({
          opacity: 1
        }, this.optSwipe),

        /* Leave page */

        animit([this.decomp.leave.content, this.decomp.leave.bottomToolbar, this.decomp.leave.background]).queue({
          transform: 'translate3d(100%, 0, 0)'
        }, this.optSwipe), animit(this.decomp.leave.toolbar).queue({
          opacity: 0
        }, this.optSwipe), animit(this.decomp.leave.toolbarCenter).queue({
          transform: 'translate3d(125%, 0, 0)'
        }, this.optSwipe), animit(this.decomp.leave.backButtonLabel).queue({
          opacity: 0,
          transform: 'translate3d(' + this.delta.title + 'px, 0, 0)',
          transition: 'opacity ' + this.durationSwipe + 's linear, transform ' + this.durationSwipe + 's ' + this.timingSwipe
        }),

        /* Other */

        animit(this.swipeShadow).queue({
          opacity: 0,
          transform: 'translate3d(' + this.maxWidth + 'px, 0, 0)'
        }, this.optSwipe).queue(function (done) {
          _this3._reset(_this3.target.enter, _this3.target.leave);
          callback && callback();
          done();
        }));
      } else {
        animit.runAll(animit(enterPage).queue({
          transform: 'translate3D(0, 0, 0)',
          opacity: 1.0
        }, this.optSwipe), animit(leavePage).queue({
          transform: 'translate3D(100%, 0, 0)'
        }, this.optSwipe).queue(function (done) {
          _this3._reset(enterPage, leavePage);
          callback && callback();
          done();
        }));
      }
    }
  }, {
    key: '_saveStyle',
    value: function _saveStyle() {
      var _this4 = this;

      this._savedStyle = new _WeakMap();
      var save = function save(el) {
        return _this4._savedStyle.set(el, el.getAttribute('style'));
      };

      for (var _len = arguments.length, args = Array(_len), _key = 0; _key < _len; _key++) {
        args[_key] = arguments[_key];
      }

      args.forEach(save);

      _Object$keys(this.decomp).forEach(function (p) {
        _Object$keys(_this4.decomp[p]).forEach(function (k) {
          (_this4.decomp[p][k] instanceof Array ? _this4.decomp[p][k] : [_this4.decomp[p][k]]).forEach(save);
        });
      });
    }
  }, {
    key: '_restoreStyle',
    value: function _restoreStyle() {
      var _this5 = this;

      var restore = function restore(el) {
        _this5._savedStyle.get(el) === null ? el.removeAttribute('style') : el.setAttribute('style', _this5._savedStyle.get(el));
        _this5._savedStyle.delete(el);
      };

      for (var _len2 = arguments.length, args = Array(_len2), _key2 = 0; _key2 < _len2; _key2++) {
        args[_key2] = arguments[_key2];
      }

      args.forEach(restore);

      _Object$keys(this.decomp).forEach(function (p) {
        _Object$keys(_this5.decomp[p]).forEach(function (k) {
          (_this5.decomp[p][k] instanceof Array ? _this5.decomp[p][k] : [_this5.decomp[p][k]]).forEach(restore);
        });
      });
    }
  }, {
    key: '_reset',
    value: function _reset() {
      this.isSwiping = false;
      this._savedStyle && this._restoreStyle.apply(this, arguments);
      this.unblock && this.unblock();
      this.swipeShadow.remove();
      this.backgroundMask.remove();
      this.overflowElement.classList.remove('overflow-visible');
      this.decomp.leave.content.classList.remove('content-swiping');
      this.decomp = this.target = this.overflowElement = this._savedStyle = null;
      this.isDragStart = true;
    }
  }]);

  return IOSSwipeNavigatorAnimator;
}(NavigatorAnimator);

export default IOSSwipeNavigatorAnimator;