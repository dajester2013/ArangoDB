/*jslint indent: 2, nomen: true, maxlen: 100, vars: true, white: true, plusplus: true*/
/*global Backbone, templateEngine, $, window */

(function() {
  "use strict";

  window.ClusterCollectionView = Backbone.View.extend({
    
    el: '#clusterCollections',

    template: templateEngine.createTemplate("clusterCollectionView.ejs"),

    events: {
      "click .collection": "loadCollection"
    },

    initialize: function() {
      this.shardsView = new window.ClusterShardsView({
        collection: new window.ClusterShards()
      });
    },

    loadCollection: function(e) {
      var id = e.currentTarget.id;
      this.shardsView.render({
        name: id
      });
    },

    unrender: function() {
      $(this.el).html("");
      this.shardsView.unrender();
    },

    render: function() {
      $(this.el).html(this.template.render({
        collections: this.collection.getList()
      }));
      this.shardsView.unrender();
      return this;
    }

  });

}());
