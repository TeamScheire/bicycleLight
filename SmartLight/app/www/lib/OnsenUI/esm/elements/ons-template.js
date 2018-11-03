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

import onsElements from '../ons/elements';
import util from '../ons/util';
import BaseElement from './base/base-element';

/**
 * @element ons-template
 * @category util
 * @description
 *   [en]
 *     Define a separate HTML fragment and use as a template. These templates can be loaded as pages in `<ons-navigator>`, `<ons-tabbar>` and `<ons-splitter>`. They can also be used to generate dialogs. Since Onsen UI 2.4.0, the native `<template>` element can be used instead of `<ons-template>` for better performance and features. `<ons-template>` is still supported for backward compatibility.
 *   [/en]
 *   [ja]テンプレートとして使用するためのHTMLフラグメントを定義します。この要素でHTMLを宣言すると、id属性に指定した名前をpageのURLとしてons-navigatorなどのコンポーネントから参照できます。[/ja]
 * @seealso ons-navigator
 *   [en]The `<ons-navigator>` component enables stack based navigation.[/en]
 *   [ja][/ja]
 * @seealso ons-tabbar
 *   [en]The `<ons-tabbar>` component is used to add tab navigation.[/en]
 *   [ja][/ja]
 * @seealso ons-splitter
 *   [en]The `<ons-splitter>` component can be used to create a draggable menu or column based layout.[/en]
 *   [ja][/ja]
 * @example
 * <ons-template id="foobar.html">
 *   <ons-page>
 *     Page content
 *   </ons-page>
 * </ons-template>
 *
 * <ons-navigator page="foobar.html"></ons-navigator>
 */

var TemplateElement = function (_BaseElement) {
  _inherits(TemplateElement, _BaseElement);

  /**
   * @property template
   * @type {String}
   * @description
   *  [en]Template content. This property can not be used with AngularJS bindings.[/en]
   *  [ja][/ja]
   */

  function TemplateElement() {
    _classCallCheck(this, TemplateElement);

    var _this = _possibleConstructorReturn(this, (TemplateElement.__proto__ || _Object$getPrototypeOf(TemplateElement)).call(this));

    _this.template = _this.innerHTML;

    while (_this.firstChild) {
      _this.removeChild(_this.firstChild);
    }
    return _this;
  }

  _createClass(TemplateElement, [{
    key: 'connectedCallback',
    value: function connectedCallback() {
      if (this.parentNode) {
        // Note: this.parentNode is not set in some CE0/CE1 polyfills.
        // Show warning when the ons-template is not located just under document.body
        if (this.parentNode !== document.body) {
          // if the parent is not document.body
          util.warn('ons-template (id = ' + this.getAttribute('id') + ') must be located just under document.body' + (this.parentNode.outerHTML ? ':\n\n' + this.parentNode.outerHTML : '.'));
        }
      }

      var event = new CustomEvent('_templateloaded', { bubbles: true, cancelable: true });
      event.template = this.template;
      event.templateId = this.getAttribute('id');

      this.dispatchEvent(event);
    }
  }]);

  return TemplateElement;
}(BaseElement);

export default TemplateElement;


onsElements.Template = TemplateElement;
customElements.define('ons-template', TemplateElement);