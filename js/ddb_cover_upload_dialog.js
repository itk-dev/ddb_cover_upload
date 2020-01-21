(function ($) {
  Drupal.behaviors.ddbCoverUpload = {
    attach: function (context, settings) {
      let copyrightLink = '.opener-modal';
      $(copyrightLink).click(function() {
        let dialogId = '#' + $(this).attr("data-dialog");
        $(dialogId).dialog({
          title: $(dialogId).attr("data-title"),
          autoOpen: false,
          resizable: false,
          draggable: false,
          modal: true
        });
        $(dialogId).dialog('open');
      });
    }
  };
}(jQuery));
