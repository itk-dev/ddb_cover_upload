/**
 * @file
 * Use drupals built in modal.
 */

(function ($) {
  Drupal.behaviors.ddbCoverUpload = {
    attach: function (context, settings) {
      let dialogLink = '.opener-modal';
      $(dialogLink).click(function() {
        let dialogId = '#' + $(this).attr("data-dialog");
        $(dialogId).dialog({
          title: $(dialogId).attr("data-title"),
          autoOpen: false,
          resizable: false,
          draggable: false,
          modal: true,
          dialogClass: 'set-size'
        });

        $(dialogId).dialog('open');
      });
    }
  };
}(jQuery));
