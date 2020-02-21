/**
 * @file
 * Perform continuous checks against cover service.
 */

(function ($) {
  let uploadAnother = '#js-upload-another';
  Drupal.behaviors.ddbCoverUpload = {
    attach: function (context, settings) {
      function checkSubmission() {
        $.ajax({
          type: 'get',
          dataType : 'json',
          url: '/admin/config/cover_upload/submitted/check-submission',
          success: function(data) {
            if (data.status === 'success') {
              setCompletedState(uploadNewHref);
            }
          },
          complete: function(data) {
            var response = JSON.parse(data.responseText);
            // Schedule the next request if we are still awaiting response.
            if(response.status === 'waiting') {
              setTimeout(checkSubmission, 1000);
            }
          }
        });
      }

      // Start loop for continuous check against cover service.
      checkSubmission();

      // Store link url and disable link.
      var uploadNewHref = $(uploadAnother).attr("href");
      $(uploadAnother).removeAttr('href');
    }
  };

  /**
   * Change frontend to visualize that upload to cover service has finished.
   *
   * @param uploadNewHref
   *   A url to page one of the form.
   */
  function setCompletedState(uploadNewHref) {
    $(uploadAnother).attr("href", uploadNewHref);
    $(uploadAnother).removeClass('disabled');
    $('.js-spinner').hide();
    $('.js-confirmation').show();
  }
}(jQuery));
