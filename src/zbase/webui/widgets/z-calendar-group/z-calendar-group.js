/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2015 Laura Schlimmer
 *   Copyright (c) 2015 Paul Asmuth
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
var CalendarGroupComponent = function() {
  this.createdCallback = function() {

  };

  
};

var proto = Object.create(HTMLElement.prototype);
CalendarGroupComponent.apply(proto);
document.registerElement("z-calendar-group", { prototype: proto });
