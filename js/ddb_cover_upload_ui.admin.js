/**
 * @file
 * Add image editor and handle file upload.
 */

(function ($) {
  Drupal.behaviors.ddbCoverUpload = {
    attach: function (context, settings) {
      var imageHasBeenUploaded = false;
      var currentImageUrl = $('input[name="image_file"]').val();

      var instance = new tui.ImageEditor(document.querySelector('#tui-image-editor'), {
        includeUI: {
          loadImage: {
            path: currentImageUrl,
            name: 'SampleImage'
          },
          //locale: locale_ru_RU,
          //theme: blackTheme, // or whiteTheme
          initMenu: 'filter',
          menuBarPosition: 'bottom'
        },
        cssMaxWidth: 700,
        cssMaxHeight: 500,
        selectionStyle: {
          cornerSize: 20,
          rotatingPointOffset: 70
        }
      });

      // Handle ajax image upload, when next is clicked.
      $('#edit-next').click(function (event) {
        let self = $(this);

        if (!imageHasBeenUploaded) {
          event.preventDefault();

          const myImage = instance.toDataURL();
          $.ajax({
            type: "POST",
            url: '/admin/config/cover_upload/ajax',
            dataType: 'text',
            data: {
              is: $('input[name="is"]').val(),
              base64data : myImage
            },
            success: function(result) {
              const json = JSON.parse(result);
              if (json.hasOwnProperty('error') && json.error === null) {
                $('input[name="image_file"]').val(json.uri);
                imageHasBeenUploaded = true;
                self.trigger('click');
              }
              else {
                alert(json.error);
              }
            }
          });
        }
        else {
          imageHasBeenUploaded = false;
        }
      });
    }


  };
}(jQuery));
