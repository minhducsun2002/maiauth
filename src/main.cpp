#include <dpp/dpp.h>
#include <cpr/cpr.h>
#include "string"

const std::string login_button_id = "lmao";
const std::string form_id = "thatform";
const std::string login_page_url = "https://lng-tgk-aime-gw.am-all.net/common_auth/login?redirect_url=https%3A%2F%2Fmaimaidx-eng.com%2Fmaimai-mobile%2F&site_id=maimaidxex&back_url=https%3A%2F%2Fmaimai.sega.com%2F";
const std::string login_endpoint = "https://lng-tgk-aime-gw.am-all.net/common_auth/login/sid/";

int main()
{
    auto token = getenv("DISCORD_TOKEN");
    dpp::cluster bot(token);
    bot.intents |= dpp::i_message_content;
    bot.intents |= dpp::i_direct_messages;
    bot.on_log(dpp::utility::cout_logger());
    
    bot.on_message_create([](const dpp::message_create_t& event) {
        if (event.msg.content == "mai!auth")
        {
            dpp::message msg;
            msg.content = "bấm cái nút này đi";
            msg.add_component(
                dpp::component()
                    .add_component(
                        dpp::component()
                            .set_label("nút để bấm")
                            .set_id(login_button_id)
                            .set_type(dpp::cot_button)
                    )
            );

            event.reply(msg);
        }
    });

    bot.on_button_click([&bot](const dpp::button_click_t & event)    {
        if (event.custom_id == login_button_id)
        {
            dpp::interaction_modal_response modal(form_id, "nhập các thứ đi");
            modal = modal.add_component(
                dpp::component()
                    .set_label("Username")
                    .set_id("username")
                    .set_type(dpp::cot_text)
                    .set_placeholder("mày là ai")
                    .set_text_style(dpp::text_short)
                )
                .add_row()
                .add_component(
                    dpp::component()
                    .set_label("Password")
                    .set_id("password")
                    .set_type(dpp::cot_text)
                    .set_placeholder("nôn mật khẩu ngân hàng ra đây")
                    .set_text_style(dpp::text_short)
                );
            event.dialog(modal);
        }
    });

    bot.on_form_submit([&bot](const dpp::form_submit_t& event) {
        if (event.custom_id == form_id)
        {
            auto user_row = event.components[0];
            auto pass_row = event.components[1];
            auto user = std::get<std::string>(user_row.components[0].value);
            auto pass = std::get<std::string>(pass_row.components[0].value);
            
            event.thinking();
            
            auto login_page = cpr::Get(cpr::Url(login_page_url));
            auto cookie = login_page.cookies;
            auto re = cpr::Post(
                cpr::Url(login_endpoint),
                cpr::Header{{ "Content-Type", "application/x-www-form-urlencoded" }},
                cookie,
                cpr::Payload{
                    {"retention", "1"},
                    {"sid", user},
                    {"password", pass}
                },
                cpr::Redirect(false)
            );
            
            if (re.header["Location"].find("https://maimaidx-eng.com") != std::string::npos)
            {
                auto c = re.cookies[0].GetValue();
                auto msg = dpp::message("cảm ơn bạn <@" + event.command.get_issuing_user().id.str() + "> đã sử dụng dịch vụ của chúng tôi");

                event.edit_original_response(msg);
                bot.message_create(
                    dpp::message("m>login clal=" + c)
                        .set_channel_id(event.command.channel_id)
                        .set_reference(event.command.message_id)
                );
            }
            else
            {
                event.edit_original_response(dpp::message("tài khoản của bạn đã bị đánh cắp"));
            }
        }
    });

    bot.on_ready([&bot](const dpp::ready_t& event) {
        auto me = bot.me;
        auto discrim_string = new char[5];
        asprintf(&discrim_string, "%04d", me.discriminator);
        bot.log(
                dpp::loglevel::ll_info,
                "We're ready! Logged in as " + me.username + "#" + std::string(discrim_string) + '.'
        );
        delete[] discrim_string;
    });

    bot.start(dpp::st_wait);
}
