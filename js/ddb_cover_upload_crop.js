/**
 * @file
 * Alter uploaded image through js.
 */

(function ($) {
  Drupal.behaviors.ddbCoverUploadCrop = {
    attach: function (context, settings) {
      window.addEventListener('load', function() {
        const img = document.getElementById('img-original');
        let cropper = null;
        if (img) {
          // Prepare canvas.
          const caman = Caman('#canvas', img.src, function () {
            cropper = $("#canvas").cropper({
              minContainerWidth: 800,
              minContainerHeight: 600,
              viewMode: 1,
              autoCropArea: 1,
              fillColor: '#000',
              crop: function (event) {
                console.log(event.detail.x);
                console.log(event.detail.y);
                console.log(event.detail.width);
                console.log(event.detail.height);
                console.log(event.detail.rotate);
                console.log(event.detail.scaleX);
                console.log(event.detail.scaleY);
              }
            }).data("cropper");
          });
          // Handle brightness and contrast buttons.
          $(".brightness, .contrast").on("click", function() {
            if (!caman) return;
            switch ($(this).attr("id")) {
              case "brightness":
                caman.brightness(10);
                break;
              case "contrast":
                caman.contrast(10);
                break;
              case "brightness-neg":
                caman.brightness(-10);
                break;
              case "contrast-neg":
                caman.contrast(-10);
                break;
            }
            // @todo Handle cropper/caman integration better. The image is
            //  replaced on each click which means you can't properly revert
            //  brightness and contrast changes.
            // https://github.com/fengyuanchen/cropper/issues/585
            caman.render(function() {
              cropper.replace(this.toBase64(), true);
            });
          });

          // Handle rotation buttons.
          $('.crop-rotate').on("click", function () {
            var degrees = $(this).attr("data-degrees");
            cropper.rotate(degrees);
          });

          // Handle mirroring.
          $('.mirror-x').on("click", function () {
            if (cropper.getData().scaleX === 1) {
              cropper.scaleX(-1);
            }
            else {
              cropper.scaleX(1);
            }
          });

          // Handle mirroring.
          $('.mirror-y').on("click", function () {
            if (cropper.getData().scaleY === 1) {
              cropper.scaleY(-1);
            }
            else {
              cropper.scaleY(1);
            }
          });

          // Handle rotation buttons.
          $('.image-reset').on("click", function () {
            caman.render(function() {
              // @todo Handle brightness and contrast reset.
              cropper.reset();
            });
          });

          // Submit modified image.
          $('#img-submit').click(function (event) {
            // @todo Do we need to find a way to handle edge browser toBlob?
            cropper.getCroppedCanvas().toBlob(function (blob) {
                var formData = new FormData();
                var fileId = $(img).data("file-id");
                formData.append('croppedImage', blob, fileId + '.jpg');
                jQuery.ajax({
                  type: 'POST',
                  url: '/cover_upload/create_alt_image',
                  data: formData,
                  processData: false,
                  contentType: false,
                  success: function () {
                    console.log('Upload success');
                    location.reload();
                  },
                  error: function () {
                    console.log('Upload error');
                  }
                });
              },
              'image/jpeg');
          });
        }

      })
    }
  };
}(jQuery));
