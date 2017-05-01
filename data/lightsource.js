$(document).ready(function () {
    stripTemplate                   = $.templates("#lightsource-strip-template");
    $.templates("lightsource-slice-template", "#lightsource-slice-template");

    $.jsonRPC.request("init", {success: remoteConfigReceived, error: remoteConfigReceptionError});

    $(".lightsource-add-strip").click(function (e) {
        console.log("onClick: .lightsource-add-strip");
        addStrip(null);
    });

    $(".lightsource-test-config").click(function (e){
        console.log("onClick: .lightsource-test-config");
        console.log($('#lightsource-config-form').serializeObject());
        $.jsonRPC.request("testConfig",
            {
                params:[$('form#lightsource-config-form').serializeObject().strips],
                success: testConfigReceived,
                error: testConfigReceptionError
            }
        );
    });

});

$.jsonRPC.setup({
    endPoint: "/devel/lightsource/data/rpc.php",
    namespace: "lightsource"
});

function addStrip(strip)
{
    console.log("addStrip:");
}

function addSlice(slice)
{
}

function createEmptySlice()
{

}

function createEmptyStrip()
{

}

function removeSlice(slice)
{
    slice.remove();
}
function removeStrip(strip)
{
    strip.remove();
}

function renderSlicesTable(strip, slices)
{
}

function renderStripsTable(strips)
{
    console.log("renderStripsTable:");
    console.log(strips);
    $("#lightsource-config-form").html(stripTemplate.render({strips: strips}));
}

function remoteConfigReceived(remoteConfig)
{
    console.log("remoteConfigReceived:");
    console.log(remoteConfig.result);

    if (remoteConfig.result.strips.length > 0)
    {
        renderStripsTable(remoteConfig.result.strips);
    }
    
    $(function()
    {
        $(".colorpicker-selector").colorpicker(
            {
                format: 'hex',
                align: 'left',
                customClass: 'colorpicker-2x',
                sliders: {
                    saturation: {
                        maxLeft: 200,
                        maxTop: 200
                    },
                    hue: {
                        maxTop: 200
                    },
                    alpha: {
                        maxTop: 200
                    }
                }
            }).on('changeColor',function(e)
            {
                $(e.target.previousElementSibling).css("background-color", e.value);
            }
        );
    });
}

function remoteConfigReceptionError(o)
{
    console.log("Error receiving initial config from remote server");
}

function testConfigReceived(o)
{
    console.log("Config test success");
}

function testConfigReceptionError(o)
{
    console.log("Config test failed");
}